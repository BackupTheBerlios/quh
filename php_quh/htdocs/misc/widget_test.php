<html>
<head>
<?php
//phpinfo();
//echo phpversion();
require_once ("misc/widget.php");

  $w = new misc_widget;
  $w->widget_init (WIDGET_CSS_BOX|WIDGET_CSS_IMG|WIDGET_CSS_A|WIDGET_CSS_SLIDER,  // css flags
                   WIDGET_JS_EVENT|WIDGET_JS_PANEL|WIDGET_JS_SLIDER|WIDGET_JS_RELATE);   // js flags

?></head><body style="background-color:AppWorkspace;">
<?php

widget_test ($w);

?></body>
</html>
