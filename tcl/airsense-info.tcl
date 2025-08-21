proc float_to_bytes {val} {
	return [format "0x%s" [exec perl -e "print pack('f>', $val)" | xxd -p]]
}

proc bytes_to_float {val_} {
	return [exec perl -e "print unpack('f>', pack('H*', '[format "%0x" $val_]'))"]
}

proc hackcheck {} {
	global IS_ORIG
	if {$IS_ORIG == 1} {
		echo "That won't work without uploading hacked firmware!"
		return 0
	}
	return 1
}
proc p {} {
	if {[hackcheck] == 0} return 
	mem2array val 32 0x20001f00 4
	set HIGHP_OVERRIDE $val(0)
	set LOWP_OVERRIDE $val(1)
	set HIGHT_OVERRIDE $val(2)
	set LOWT_OVERRIDE $val(3)
	#echo [format "override registers: HP=0x%x LP=0x%x HT=0x%x LT=0x%x" $HIGHP_OVERRIDE $LOWP_OVERRIDE $HIGHT_OVERRIDE $LOWT_OVERRIDE]
	if {$LOWP_OVERRIDE != 0} {
		echo [format "low pressure: %.1f cm-h2O (set by debugger)" [eval bytes_to_float $LOWP_OVERRIDE]]
	} else {
		echo "low pressure: 4.0 cm-h2O (default)"
	}
	if {$HIGHP_OVERRIDE != 0} {
		echo [format "high pressure: %.1f cm-h2O (set via debugger)" [eval bytes_to_float $HIGHP_OVERRIDE]]
	} else {
		echo [format "high pressure: %.1f cm-h2O (set by clinician menu)" [eval bytes_to_float [mrw 0x2000e96c]]]
	}
	set ms_per_breath [expr {.25 * [bytes_to_float [mrw 0x2000e970]]}]
	if {$HIGHT_OVERRIDE != 0} {
		echo [format "high pressure time: %.1f seconds (set via debugger)" [expr {.000001 * [mrw 0x20001f08]}]]
	} else {
		echo [format "high pressure time: %.1f seconds (set via clinician menu)" $ms_per_breath]
	}
	if {$LOWT_OVERRIDE != 0} {
		echo [format "low pressure time: %.1f seconds (set via debugger)" [expr {.000001 * [mrw 0x20001f0c]}]]
	} else {
		echo [format "low pressure time: %.1f seconds (set via clinician menu)" $ms_per_breath]
	}
}

proc lp {arg} {
	if {[hackcheck] == 0} return 
	echo "Setting low pressure value to $arg cm-h2O..."
	mww 0x20001f04 [float_to_bytes $arg]
	p
}

proc hp {arg} {
	if {[hackcheck] == 0} return 
	echo "Setting high pressure value to $arg cm-h2O..."	
	mww 0x20001f00 [float_to_bytes $arg]
	p
}

proc ht {arg} {
	if {[hackcheck] == 0} return 
	echo "Setting high time to $arg seconds..."	
	set ms_tm [format %d [expr {round($arg * 1000000)}]]
	mww 0x20001f08 [format 0x%08x $ms_tm]
	p
}
proc lt {arg} {
	if {[hackcheck] == 0} return 
	echo "Setting low time to $arg seconds..."	
	set ms_tm [format %d [expr {round($arg * 1000000)}]]
	mww 0x20001f0c [format 0x%08x $ms_tm]
	p
}

proc ra {} {
	if {[hackcheck] == 0} return 
	echo "Clearing setting overrides..."
	mwd 0x20001f00 0
	mwd 0x20001f08 0
	p
}

proc h {} {
	echo "Airsense S10 Custom Firmware Debug Commands:"
	echo "\tlt \[val\] : set low pressure time interval (seconds)"
	echo "\tht \[val\] : set high pressure time interval (seconds)"
	echo "\thp \[val\] : set high pressure value (cm-h2O, 0-30))"
	echo "\tlp \[val\] : set low pressure value (cm-h2O, 0-30))"
	echo "\tra       : reset override values to clinician menu settings"
	echo "\tp        : print all values"
	echo "\th        : show this help screen"
}

# ------------------------------------------------------------------
# Faycal Kilalis Postboot fix

proc postboot {} {
    # resume execution at the reset vector
    echo "Jumping to application…"
    reset run
}
# ------------------------------------------------------------------
