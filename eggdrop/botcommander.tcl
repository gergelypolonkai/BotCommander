bind chon - * botcommander:chon
bind dcc - botcomm_chanlist botcommander:chanlist
bind dcc - botcomm_userlist botcommander:userlist
bind dcc - botcomm_chanprop botcommander:chanprop

proc botcommander:chon {handle connid} {
	putdcc $connid "BC+ ABILITY\r\n"
}

proc botcommander:chanlist {handle connid params} {
	if {[matchattr $handle o]} {
		putdcc $connid "BC+ CHANLIST_START\r\n"
		foreach chan [channels] {
			set active [channel get $chan inactive]
			if {$active != 0} { set active 0 } else { set active 1 }
			putdcc $connid "BC+ CHAN $active $chan\r\n"
		}
		putdcc $connid "BC+ CHANLIST_END\r\n"
	} else {
		putcc $connid "BC- NOPRIV\r\n"
	}
}

proc botcommander:userlist {handle connid params} {
	if {[matchattr $handle o]} {
		putdcc $connid "BC+ USERLIST_START\r\n"
		foreach user [userlist] {
			putdcc $connid "BC+ USER $user\r\n"
		}
		putdcc $connid "BC+ USERLIST_END\r\n"
	} else {
		putdcc $connid "BC- NOPRIV\r\n"
	}
}

proc botcommander:chanprop {handle connid params} {
	if {[matchattr $handle o]} {
		set chan [lindex $params 0]
		putdcc $connid "BC+ CHANPROPS_START $chan\r\n"
		foreach prop [channel info $chan] {
			putdcc $connid "BC+ CHANPROP $chan $prop\r\n"
		}
		putdcc $connid "BC+ CHANPROPS_END $chan\r\n"
	} else {
		putdcc $connid "BC- NOPRIV"
	}
}

putlog "BC+ ABILITY"

