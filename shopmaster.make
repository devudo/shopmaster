core = 6.x
api = 2

projects[drupal][type] = "core"

; shop_hosting
projects[shop_hosting][type] = "module"
projects[shop_hosting][download][type] = "git"
projects[shop_hosting][download][url] = "git@github.com:devudo/shop_hosting.git"
projects[shop_hosting][download][branch] = "master"

; Contrib
projects[sshkey][type] = "module"
projects[cck][type] = "module"
projects[features][type] = "module"
projects[views][type] = "module"

; HOSTMASTER CORE
projects[hostmaster][type] = "profile"

; Contrib modules
projects[admin_menu][version] = "1.8"
projects[openidadmin][version] = "1.2"
projects[install_profile_api][version] = "2.1"
projects[jquery_ui][version] = "1.3"
projects[modalframe][version] = "1.6"

; Libraries
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][destination] = "modules/jquery_ui"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery.ui-1.6.zip"
libraries[jquery_ui][directory_name] = "jquery.ui"
