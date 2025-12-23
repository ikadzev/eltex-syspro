.PHONY: exec clear
exec: server client

%: %.c
	gcc $< -o $@

clear:
	rm server client
