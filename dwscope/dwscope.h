/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DWSCOPE.H */
/*  *4     8-APR-1993 10:59:23 TWF "Remove variant unions" */
/*  *3    24-FEB-1993 12:36:02 TWF "No VARIANT's" */
/*  *2    25-NOV-1991 10:32:07 TWF "add show mode" */
/*  *1    12-NOV-1991 14:55:30 TWF "Data structures for DWSCOPE" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DWSCOPE.H */
typedef struct _WaveInfo
{
  Boolean update;
  Boolean x_grid_labels;
  Boolean y_grid_labels;
  char show_mode;
  Boolean step_plot;
  int x_grid_lines;
  int y_grid_lines;
  String database;
  String shot;
  String default_node;
  String x;
  String y;
  String xmin;
  String xmax;
  String ymin;
  String ymax;
  String title;
  String event;
  String print_title;
  String pad_label;
  union {
    int global_defaults;
    struct
    {
      unsigned update : 1;
#define B_update 0
#define M_update (1 << B_update)
      unsigned x_grid_labels : 1;
#define B_x_grid_labels 1
#define M_x_grid_labels (1 << B_x_grid_labels)
      unsigned y_grid_labels : 1;
#define B_y_grid_labels 2
#define M_y_grid_labels (1 << B_y_grid_labels)
      unsigned show_mode : 1;
#define B_show_mode 3
#define M_show_mode (1 << B_show_mode)
      unsigned step_plot : 1;
#define B_step_plot 4
#define M_step_plot (1 << B_step_plot)
      unsigned x_grid_lines : 1;
#define B_x_grid_lines 5
#define M_x_grid_lines (1 << B_x_grid_lines)
      unsigned y_grid_lines : 1;
#define B_y_grid_lines 6
#define M_y_grid_lines (1 << B_y_grid_lines)
      unsigned database : 1;
#define B_database 7
#define M_database (1 << B_database)
      unsigned shot : 1;
#define B_shot 8
#define M_shot (1 << B_shot)
      unsigned default_node : 1;
#define B_default_node 9
#define M_default_node (1 << B_default_node)
      unsigned x : 1;
#define B_x 10
#define M_x (1 << B_x)
      unsigned y : 1;
#define B_y 11
#define M_y (1 << B_y)
      unsigned xmin : 1;
#define B_xmin 12
#define M_xmin (1 << B_xmin)
      unsigned xmax : 1;
#define B_xmax 13
#define M_xmax (1 << B_xmax)
      unsigned ymin : 1;
#define B_ymin 14
#define M_ymin (1 << B_ymin)
      unsigned ymax : 1;
#define B_ymax 15
#define M_ymax (1 << B_ymax)
      unsigned title : 1;
#define B_title 16
#define M_title (1 << B_title)
      unsigned event : 1;
#define B_event 17
#define M_event (1 << B_event)
      unsigned print_title : 1;
#define B_print_title 18
#define M_print_title (1 << B_print_title)
      unsigned pad_label : 1;
#define B_pad_label 19
#define M_pad_label (1 << B_pad_label)
    } global;
  } _global;
  Dimension height;
  Widget w;
  void *eventid;
  Boolean received;
  String print_title_evaluated;
} WaveInfo;

#define DefaultDefaults                                                       \
  (M_update | M_x_grid_labels | M_y_grid_labels | M_show_mode | M_step_plot | \
   M_x_grid_lines | M_y_grid_lines | M_database | M_shot | M_default_node |   \
   M_xmin | M_xmax | M_ymin | M_ymax)

typedef struct _CutHeader
{
  Boolean update;
  Boolean x_grid_labels;
  Boolean y_grid_labels;
  char show_mode;
  Boolean step_plot;
  int x_grid_lines;
  int y_grid_lines;
  short database_length;
  short shot_length;
  short default_node_length;
  short x_length;
  short y_length;
  short xmin_length;
  short xmax_length;
  short ymin_length;
  short ymax_length;
  short title_length;
  short event_length;
  short print_title_length;
  short pad_label_length;
  int global_defaults;
} CutHeader;
