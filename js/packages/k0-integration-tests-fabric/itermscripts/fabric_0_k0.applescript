tell application "System Events" to keystroke "t" using command down
set tree_height to 4

tell application "iTerm"
	activate
	select first window
	
	tell current session of current window
		split horizontally with default profile
		split horizontally with default profile
	end tell
	
	tell session 1 of current tab of current window
		split vertically with default profile
		split vertically with default profile
		split vertically with default profile
	end tell
	
	tell session 5 of current tab of current window
		split vertically with default profile
		split vertically with default profile
		split vertically with default profile
	end tell
	
	my start_server(1, tree_height, 11400)
	my start_server(2, tree_height, 12400)
	my start_server(3, tree_height, 13400)
	my start_server(4, tree_height, 14400)
	
	my start_mtserver(5, tree_height, 11410)
	my start_mtserver(6, tree_height, 12410)
	my start_mtserver(7, tree_height, 13410)
	my start_mtserver(8, tree_height, 14410)
end tell

on start_server(session_num, tree_height, port)
	my run_zktrading_command(session_num, "cpp/build/src/server " & tree_height & " /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk " & port)
end start_server

on start_mtserver(session_num, tree_height, port)
	my run_zktrading_command(session_num, "cpp/build/src/mtserver " & tree_height & " " & port)
end start_mtserver

on run_zktrading_command(session_num, cmd)
	tell application "iTerm"
		tell session session_num of current tab of current window
			write text "cd ~/go/src/github.com/appliedblockchain/zktrading"
			write text cmd
		end tell
	end tell
end run_zktrading_command