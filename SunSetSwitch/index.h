String getBody() {
	return "\
<nav class='light-blue lighten-1' role='navigation'>\n\
  <div class='nav-wrapper container'><a id='logo-container' href='#' class='brand-logo'>Sun Set Switch</a>\n\
    <ul class='right hide-on-med-and-down'>\n\
      <li><a href='https://github.com/jazeee/esp8266/tree/master'>GitHub</a></li>\n\
    </ul>\n\
\n\
    <ul id='nav-mobile' class='sidenav'>\n\
      <li><a href='https://github.com/jazeee/esp8266/tree/master'>GitHub</a></li>\n\
    </ul>\n\
    <a href='#' data-target='nav-mobile' class='sidenav-trigger'><i class='material-icons'>menu</i></a>\n\
  </div>\n\
</nav>\n\
<div class='section no-pad-bot' id='index-banner'>\n\
  <div class='container'>\n\
    <br><br>\n\
    <h1 class='header center orange-text'>Jaz's Sun Set Switch</h1>\n\
    <div class='row center'>\n\
      <h5 class='header col s12 light'>This will turn on at sun-set, and off around 10PM</h5>\n\
    </div>\n\
    <div class='row'>\n\
	    <div class='col m2 s12 center'></div>\n\
	    <div class='col m3 s12 center'>\n\
	      <a href='#' onclick='toggleSwitch()' id='toggle-switch' class='btn-large waves-effect waves-light orange'>\n\
	        Toggle Switch \n\
	      </a>\n\
      </div>\n\
	    <div class='col m2 s12 center'></div>\n\
	    <div class='col m3 s12 center'>\n\
	      <a href='#' onclick='getRelayTime()' id='toggle-switch' class='btn-large waves-effect waves-light orange'>\n\
	        Get Time\n\
	      </a>\n\
      </div>\n\
	    <div class='col m2 s12 center'></div>\n\
    </div>\n\
    <div class='row center'>\n\
      <h5 class='header col s12 light' id='switch-response'></h5>\n\
    </div>\n\
    <br><br>\n\
\n\
  </div>\n\
</div>\n\
";
}

String getScripts() {
	return "\
<script>\n\
  function toggleSwitch() {\n\
    fetch(\"/toggle-relay\", {method: 'POST'}).then(response=>response.text())\n\
    .then((text) => {\n\
      console.log(text);\n\
      document.getElementById('switch-response').textContent = text;\n\
    });\n\
  }\n\
  function getRelayTime() {\n\
    fetch(\"/time\", {method: 'GET'}).then(response=>response.text())\n\
    .then((text) => {\n\
      console.log(text);\n\
      document.getElementById('switch-response').textContent = text;\n\
    });\n\
  }\n\
</script>\n\
	";
}
String getIndexHtml() {
	return String(
"<!DOCTYPE html>\n"
"<html>\n\
  <head>\n\
    <!--Import Google Icon Font-->\n\
    <link href='https://fonts.googleapis.com/icon?family=Material+Icons' rel='stylesheet'>\n\
    <!-- Compiled and minified CSS -->\n\
    <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css' \n\
    media='screen,projection'>\n\
\n\
    <!--Let browser know website is optimized for mobile-->\n\
    <meta name='viewport' content='width=device-width, initial-scale=1.0'/>\n\
  </head>\n\
\n\
  <body>\n" +
  getBody() +
  "\n" + 
  getScripts() + "\
    <!--JavaScript at end of body for optimized loading-->\n\
    <!-- Compiled and minified JavaScript -->\n\
    <script src='https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/js/materialize.min.js'></script>	\n\
  </body>\n\
</html>\n\
	");
}
