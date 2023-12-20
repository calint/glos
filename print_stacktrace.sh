cat stacktrace | awk '{print $2}'|cut -c 2-9 | while read addr; do
	addr2line -e glos $addr
done