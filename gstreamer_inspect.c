/* compile with:
 * gcc -o tags tags.c `pkg-config --cflags --libs gstreamer-0.10` */
#include <gst/gst.h>

static void
print_one_tag (const GstTagList * list, const gchar * tag, gpointer user_data)
{
  int i, num;

  num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i < num; ++i) {
    const GValue *val;

    /* Note: when looking for specific tags, use the g_tag_list_get_xyz() API,
     * we only use the GValue approach here because it is more generic */
    val = gst_tag_list_get_value_index (list, tag, i);
    if (G_VALUE_HOLDS_STRING (val)) {
      g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
    } else if (G_VALUE_HOLDS_UINT (val)) {
      g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
    } else if (G_VALUE_HOLDS_DOUBLE (val)) {
      g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
    } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
      g_print ("\t%20s : %s\n", tag,
          (g_value_get_boolean (val)) ? "true" : "false");
    } else if (GST_VALUE_HOLDS_BUFFER (val)) {
      g_print ("\t%20s : buffer of size %u\n", tag,
          GST_BUFFER_SIZE (gst_value_get_buffer (val)));
    } else if (GST_VALUE_HOLDS_DATE (val)) {
      g_print ("\t%20s : date (year=%u,...)\n", tag,
          g_date_get_year (gst_value_get_date (val)));
    } else {
      g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
  }
}

static void
on_new_pad (GstElement * dec, GstPad * pad, GstElement * fakesink)
{
  GstPad *sinkpad;

  sinkpad = gst_element_get_static_pad (fakesink, "sink");
  if (!gst_pad_is_linked (sinkpad)) {
    if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK)
      g_error ("Failed to link pads!");
  }
  gst_object_unref (sinkpad);
}

int
main (int argc, char ** argv)
{
  GstElement *pipe, *dec, *sink;
  GstMessage *msg;

  gst_init (&argc, &argv);

  pipe = gst_pipeline_new ("pipeline");
  GstElement* src = gst_element_factory_make("filesrc", NULL);
  //dec = gst_element_factory_make ("uridecodebin", NULL);
  dec = gst_element_factory_make ("decodebin2", NULL);
  sink = gst_element_factory_make ("fakesink", NULL);
  
  gst_bin_add_many(GST_BIN(pipe), src, dec, sink, NULL);
  gst_element_link_pads(src, "src", dec, "sink");
  g_signal_connect (dec, "pad-added", G_CALLBACK (on_new_pad), sink);

  
  for (int i = 1; i < argc; i++){
    char* filename = canonicalize_file_name(argv[i]);

    g_object_set(src, "location", filename, NULL);
    gst_element_set_state (pipe, GST_STATE_PAUSED);

    g_print("Tags from %s:\n", filename);
    
    while (TRUE) {
      GstTagList *tags = NULL;

      msg = gst_bus_timed_pop_filtered (GST_ELEMENT_BUS (pipe),
	  GST_CLOCK_TIME_NONE,
	  GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_TAG | GST_MESSAGE_ERROR);

      if (GST_MESSAGE_TYPE (msg) != GST_MESSAGE_TAG) /* error or async_done */
	break;

      gst_message_parse_tag (msg, &tags);

      g_print ("Got tags from element %s:\n", GST_OBJECT_NAME (msg->src));
      gst_tag_list_foreach (tags, print_one_tag, NULL);
      g_print ("\n");
      gst_tag_list_free (tags);

      gst_message_unref (msg);
    };

    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR){
      GError* error;
      gst_message_parse_error(msg, &error, NULL);
      g_print("Got error: %s\n", error->message);
      g_error_free(error);
      g_error ("Bye!\n");
      
    }

    gst_message_unref (msg);
    gst_element_set_state (pipe, GST_STATE_NULL);  
    free(filename);
  }
  
  gst_object_unref (pipe);
  return 0;
}
    