<?php
require_once ("misc/widget.php");


  $w = new misc_widget;
  $w->widget_init (0, // css flags
                   WIDGET_JS_WINDOW); // js flags
?>

<a href="javascript:js_window_open ('php_quh.php',
                                    'mywindow',
/*                                    'fullscreen=yes,'
*/                                  'width=415,'
                                   +'height=650,'
                                   +'resizable=no,'
                                   +'scrollbars=no,'
                                   +'toolbar=no,'
                                   +'location=no,'
                                   +'directories=no,'
                                   +'status=no,'
                                   +'menubar=no,'
                                   +'copyhistory=no');">Start</a>
