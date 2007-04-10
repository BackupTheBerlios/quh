<?php

class configure
{
  var $agent = NULL;

  var $have_ns4 = 0;
  var $have_ns6 = 0;
  var $have_ie = 0;
  var $have_op = 0;
  var $have_other = 0;

  var $have_css = 0;
  var $have_js = 0;
  var $have_flash = 0;


function
configure_new ()
{
  $p = "";

  if (isset ($_SERVER['HTTP_USER_AGENT']))
    {
      $user_agent = strtolower ($_SERVER['HTTP_USER_AGENT']);

      /*
        the order is important, because opera must be tested first, and ie4 tested
        for before ie general same for konqueror, then safari, then gecko, since
        safari navigator user agent id's with 'gecko' in string.
      */
      if (stristr ($user_agent, "opera")) 
        $p = "opera";
      elseif (stristr ($user_agent, "msie 4")) 
        $p = "msie4"; 
      elseif (stristr ($user_agent, "msie")) 
        $p = "msie"; 
      elseif ((stristr ($user_agent, "konqueror")) ||
              (stristr ($user_agent, "safari"))) 
        $p = "safari"; 
      elseif (stristr ($user_agent, "gecko")) 
        $p = "mozilla";
      elseif (stristr ($user_agent, "mozilla/4")) 
        $p = "ns4";
      else 
        $p = false;

      return $p;
    }

  return NULL;
}


function
configure ()
{
  $this->agent = $_SERVER["HTTP_USER_AGENT"];

  if (stristr ($this->agent, "Netscape"))
    $this->have_ns4 = 1;
  else if (stristr ($this->agent, "Mozilla"))
    $this->have_ns6 = 1;
  else if (stristr ($this->agent, "Microsoft"))
    $this->have_ie = 1;
  else if (stristr ($this->agent, "Opera"))
    $this->have_op = 1;
  else
    $this->have_other = 1;

  $this->have_css = 1;
  $this->have_js = 1;
  $this->have_flash = 1;
}


function
get_js ()
{
  return "is_ns4 = "
        .($this->have_ns4 ? 1 : 0)
        .";\n"
        ."is_ns6 = "
        .($this->have_ns6 ? 1 : 0)
        .";\n"
        ."is_ie = "
        .($this->have_ie ? 1 : 0)
        .";\n"
        ."is_op5 = "
        .($this->have_op ? 1 : 0)
        .";\n"
        ."is_other = "
        .($this->have_other ? 1 : 0)
        .";\n"
        ."is_css = "
        .($this->have_css ? 1 : 0)
        .";\n"
        ."is_js = "
        .($this->have_js ? 1 : 0)
        .";\n"
        ."is_flash = "
        .($this->have_flash ? 1 : 0)
        .";\n";
}

}


?>
