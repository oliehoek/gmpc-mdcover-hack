#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <regex.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gmpc/plugin.h>
#include <libmpd/libmpd.h>
#include <libmpd/debug_printf.h>

int fetch_get_image(mpd_Song *song,MetaDataType type, char **path);

void music_dir_cover_art_pref_construct(GtkWidget *container);
void music_dir_cover_art_pref_destroy(GtkWidget *container);
GList * fetch_cover_art_path_list(mpd_Song *song);
int fetch_cover_art_path(mpd_Song *song, gchar **path);
static GtkWidget *wp_pref_vbox = NULL;


/*
 * Enable/Disable state of the plugins
 */
static int mdca_get_enabled()
{
	return cfg_get_single_value_as_int_with_default(config, "music-dir-cover", "enable", TRUE);
}
static void mdca_set_enabled(int enabled)
{
	cfg_set_single_value_as_int(config, "music-dir-cover", "enable", enabled);
}
/* priority */
static int fetch_cover_priority()
{
				return cfg_get_single_value_as_int_with_default(config, "music-dir-cover", "priority", 10);
}
/* Plugin structure(s) */ 
gmpcPrefPlugin mdca_pref = {
	music_dir_cover_art_pref_construct,
	music_dir_cover_art_pref_destroy
};

gmpcMetaDataPlugin mdca_cover = {
	fetch_cover_priority,
	fetch_get_image
};
/* a workaround so gmpc has some sort of api checking */
int plugin_api_version = PLUGIN_API_VERSION;
/* the plugin */
gmpcPlugin plugin = {
	"Music Dir Fetcher",
	{0,15,0},
	GMPC_PLUGIN_META_DATA,
	0,
	NULL, /* path  */
	NULL, /* init  */
	NULL, /* destroy */
	NULL, /* browser */
	NULL, /* status changed */
	NULL, /* connection changed */
	&mdca_pref, /* preferences */
	&mdca_cover, /* meta data */
	mdca_get_enabled,
	mdca_set_enabled
};


int fetch_get_image(mpd_Song *song,MetaDataType type, char **path)
{
				if(song  == NULL || song->file == NULL)
				{
								return META_DATA_UNAVAILABLE;
				}
				if(type == META_ALBUM_ART)
				{
								int retv = fetch_cover_art_path(song,path);
								if(retv == META_DATA_AVAILABLE)
								{
												return META_DATA_AVAILABLE;
								}
								else
								{
												if(*path) g_free(*path);
												return META_DATA_UNAVAILABLE;
								}
				}
				else if(type == META_SONG_TXT)
				{
					if(song->file)
					{
						gchar *musicroot= cfg_get_single_value_as_string(config, "music-dir-cover", "musicroot");
						gchar *file = g_malloc0((strlen(musicroot)+strlen(song->file)+strlen("lyrics")+2)*sizeof(*file));
						int length = strlen(song->file);
						strcat(file, musicroot);
						g_free(musicroot);
						strcat(file, G_DIR_SEPARATOR_S);
						for(;length > 0 && song->file[length] != '.';length--);
						strncat(file, song->file, length+1);
						strcat(file, "lyric");	
						if(g_file_test(	file, G_FILE_TEST_EXISTS))
						{
							*path = file;

							return META_DATA_AVAILABLE;
						}
						g_free(file);
						return META_DATA_UNAVAILABLE;
					}

				}
				else if(type == META_ARTIST_ART || type == META_ARTIST_TXT|| type == META_ALBUM_TXT)
				{
					char *extention = NULL;
					char *name = NULL;
					switch(type)
					{
						case META_ARTIST_TXT:
							name = "BIOGRAPHY";
							extention = "";
							break;
						case META_ALBUM_TXT:
							name = song->album;
							extention = ".txt";
							break;
						default:
							name = song->artist;
							extention = ".jpg";
							break;
					}
					if(song->artist)
					{
						gchar *musicroot= cfg_get_single_value_as_string(config, "music-dir-cover", "musicroot");
						gchar **dirs = NULL;
						gchar *fpath = NULL;
						gchar *song_path = NULL;
						int i = 0;
						if(!musicroot) return META_DATA_UNAVAILABLE;
						song_path = g_path_get_dirname(song->file);
						for(i=strlen(song_path); i >= 0 && *path == NULL;i--)
						{
							if(song_path[i] == G_DIR_SEPARATOR)
							{	
								song_path[i] = '\0';
								fpath = g_strdup_printf("%s%c%s%c%s%s",
										musicroot, G_DIR_SEPARATOR,
										song_path, G_DIR_SEPARATOR,
										name,
										extention);
								if(g_file_test(	fpath, G_FILE_TEST_EXISTS))
								{
									*path = fpath;
								}
								else
								{
									g_free(fpath);
								}
							}
						}
						g_free(song_path);
						cfg_free_string(musicroot);
						if(*path)
						{
							return META_DATA_AVAILABLE;
						}
					}
				}

				return META_DATA_UNAVAILABLE;
}

int fetch_cover_art_path(mpd_Song *song, gchar **path)
{
	GList *list = NULL, *node = NULL;	
	node = list = fetch_cover_art_path_list(song);
	*path = NULL;
	if(list == NULL)
	{
		return META_DATA_UNAVAILABLE;
	}
	/* check for image with name "cover/voorkant/front/large/folder" */
	regex_t regt;
	if(!regcomp(&regt,"(voorkant|front|cover|large|folder)", REG_EXTENDED|REG_ICASE))
	{                                                                	
		do{
			char *data = node->data;
			if(!regexec(&regt, data,0,NULL,0))
			{
				*path = g_strdup(data);
				regfree(&regt);
				g_list_foreach(list, (GFunc)g_free,NULL);
				g_list_free(list);	
				return META_DATA_AVAILABLE;                                     				
			}
		}while((node = g_list_next(node)));
	}
	*path = g_strdup(list->data);
	regfree(&regt);
	g_list_foreach(list, (GFunc)g_free,NULL);
	g_list_free(list);	
	return META_DATA_AVAILABLE;
}

void fetch_cover_art_path_list_from_dir(gchar *url, GList **list)
{

	GDir *dir = g_dir_open(url,0,NULL);
	if(dir)
	{
		const char *filename;
		regex_t regt;
		if(!regcomp(&regt,"(png|jpg|jpeg|gif)$", REG_EXTENDED|REG_ICASE))
		{
			filename = g_dir_read_name(dir);
			do{
				/* ignore . files */
				if(filename[0] != '.' || !strncmp(filename, ".folder",7))
				{
					if(!regexec(&regt, filename, 0,NULL,0))	
					{
						char *path = g_strdup_printf("%s%c%s", url,G_DIR_SEPARATOR,filename);
						*list = g_list_append(*list, path);
					}
				}
				filename = g_dir_read_name(dir);
			}while(filename);
		}
		regfree(&regt);
		g_dir_close(dir);
	}
}
GList * fetch_cover_art_path_list(mpd_Song *song) 
{
	char *url =NULL;
	gchar *musicroot = NULL;
	char *dirname = NULL;
	GList *list = NULL;
	GDir *dir = NULL;
	regex_t regt;


	if(song == NULL || !cfg_get_single_value_as_int_with_default(config, "music-dir-cover", "enable", TRUE))
	{
		return NULL;
	}
	if(song->file == NULL )
	{
		return NULL;
	}

	musicroot= cfg_get_single_value_as_string(config, "music-dir-cover", "musicroot");
	if(musicroot == NULL)
	{
		debug_printf(DEBUG_WARNING, "No Music Root");
		return NULL;
	}
	dirname = g_path_get_dirname(song->file);
	if(dirname == NULL)
	{
		debug_printf(DEBUG_WARNING, "Cannot get file's directory name");
		cfg_free_string(musicroot);
		return NULL;
	}	
	if(song->album)
	{
		int i = 0;
		char *album = g_strdup(song->album);
		for(i=0;i<strlen(album);i++) if(album[i] == G_DIR_SEPARATOR) album[i] = ' ';
		/* Test the default "Remco" standard */	
		url = g_strdup_printf("%s%c%s%c%s.jpg",
				musicroot,
				G_DIR_SEPARATOR,
				dirname,
				G_DIR_SEPARATOR,
				album
				);
		g_free(album);
		if(g_file_test(url, G_FILE_TEST_EXISTS))
		{
			list = g_list_append(list, url);	
		}
		else g_free(url);
	}
	/* Ok, not found, now let's do it brute-force */

	url = g_strdup_printf("%s/%s/",
			musicroot,
			dirname
			);
	fetch_cover_art_path_list_from_dir(url, &list);
	g_free(url);

	if(!regcomp(&regt,"(CD|DISC) [0-9]$", REG_EXTENDED|REG_ICASE))
	{
		if(!regexec(&regt, dirname,0,NULL,0))
		{
			char *new_dir = NULL;
			int i;
			for(i= strlen(dirname);i> 0 && dirname[i] != G_DIR_SEPARATOR; i--);
			new_dir = g_strndup(dirname, i);
			url = g_strdup_printf("%s%c%s%c", musicroot,G_DIR_SEPARATOR, new_dir,G_DIR_SEPARATOR);
			debug_printf(DEBUG_INFO,"Trying: %s\n", url);
			fetch_cover_art_path_list_from_dir(url, &list);
			g_free(url);
			g_free(new_dir);
		}
	}



	g_free(dirname);
	cfg_free_string(musicroot);
	return g_list_first(list);
}


void music_dir_cover_art_enable_toggle(GtkWidget *wid)
{
	int kk = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wid));
	cfg_set_single_value_as_int(config, "music-dir-cover", "enable", kk);
}
void music_dir_cover_art_pref_destroy(GtkWidget *container)
{
	gtk_container_remove(GTK_CONTAINER(container), wp_pref_vbox);
}
static void info_entry_edited(GtkWidget *entry)
{
	const char *str = gtk_entry_get_text(GTK_ENTRY(entry));
	if(str)
	{
		cfg_set_single_value_as_string(config, "music-dir-cover", "musicroot",(char *)str);
	}
}

void music_dir_cover_art_pref_construct(GtkWidget *container)
{
	GtkWidget *enable_cg = gtk_check_button_new_with_mnemonic("_Enable mpd's music dir as cover art source");
	GtkWidget *entry = NULL;
	char *entry_str = cfg_get_single_value_as_string(config, "music-dir-cover", "musicroot");
	wp_pref_vbox = gtk_vbox_new(FALSE,6);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enable_cg), 	
			cfg_get_single_value_as_int_with_default(config, "music-dir-cover", "enable", TRUE));

	g_signal_connect(G_OBJECT(enable_cg), "toggled", G_CALLBACK(music_dir_cover_art_enable_toggle), NULL);
	gtk_box_pack_start(GTK_BOX(wp_pref_vbox), enable_cg, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(container), wp_pref_vbox);

	entry = gtk_entry_new();
	if(entry_str)
	{
		gtk_entry_set_text(GTK_ENTRY(entry), entry_str);
		cfg_free_string(entry_str);
	}
	gtk_box_pack_start(GTK_BOX(wp_pref_vbox), gtk_label_new("Music Root:"), FALSE, FALSE,0);
	gtk_box_pack_start(GTK_BOX(wp_pref_vbox), entry, FALSE, FALSE,0);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(info_entry_edited), NULL);

	gtk_widget_show_all(container);
}
