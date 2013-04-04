; NOTE!  Since the name shopmaster matches the project (profile),
; anything in here goes into profiles/shopmaster/modules etc!

core = 6.x
api = 2

projects[drupal][type] = "core"

; Contrib
projects[adminrole][subdir] = "contrib"
projects[sshkey][subdir] = "contrib"

projects[cck][subdir] = "contrib"
projects[cck][version] = "2.9"

projects[features][subdir] = "contrib"

projects[views][subdir] = "contrib"

; Dev
projects[devel][subdir] = "devel"


; HOSTMASTER CORE
; This automatically inherits hostmaster's make file
; we don't have to worry about anything already included in hostmaster
projects[hostmaster][type] = "module"
projects[views][subdir] = "hostmaster"

