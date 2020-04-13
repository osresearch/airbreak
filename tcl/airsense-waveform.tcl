proc bytearr_to_float {val} {
	return [exec perl -e "print join(', ',unpack('f>*', pack('H*', '$val')));"]
}

proc floatconv {f buf} {
	if {$f eq ""} {
		echo [bytearr_to_float $buf]
	} else { 
		puts $f [bytearr_to_float $buf]
	}
}

proc dump_float_table {offset numfloats numsamples delay {fname ""} args} {
	if {$fname eq ""} {
		set f ""
		echo "Printing float table."
	} else {
		set f [open $fname w]
		echo "Dumping float table to $fname"
	}
	set buffer ""
	for {set j 0} {$j < $numsamples} {incr j} {
		mem2array val 32 $offset $numfloats
		for {set i 0} {$i < $numfloats} {incr i} {
			append buffer [format "%08x" $val($i)]
		}
		floatconv $f $buffer
		set buffer ""
		after $delay 
	}
	if {$fname != ""} { 
		close $f
	}
	echo done!
}

# example: dump_therapy_values 16 100 50 therapy_vals.csv
# without providing an fname, it will echo to console
proc dump_therapy_values {numfloats numsamples delay {fname ""} } {
	dump_float_table 0x2000e96c $numfloats $numsamples $delay $fname

}

