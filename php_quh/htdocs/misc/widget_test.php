<html>
<head>
<?php
//phpinfo();
//echo phpversion();
require_once ("misc/widget.php");

  $w = new misc_widget;
  $w->widget_init (WIDGET_CSS_BOX|WIDGET_CSS_IMG|WIDGET_CSS_A,  // css flags
                   WIDGET_JS_MOUSE|WIDGET_JS_PANEL);   // js flags

?></head><body style="background-color:AppWorkspace;">
<?php

widget_test ($w);

?></body>
</html>
