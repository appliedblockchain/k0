tell application "System Events" to keystroke "t" using command down
tell application "iTerm"
	activate
	select first window
	
	tell current session of current window
		write text "cd $GOPATH/src/github.com/appliedblockchain/zktrading/js/packages/k0-integration-tests-fabric/devnetwork"
		write text "./start.sh &"
		split horizontally with default profile
		split horizontally with default profile
	end tell
	
	repeat with sessionNumber in {2, 6}
		tell session sessionNumber of current tab of current window
			split vertically with default profile
			split vertically with default profile
			split vertically with default profile
		end tell
	end repeat
	
	tell session 1 of current tab of current window
		write text "wait"
		write text "cd alphaadmin"
		write text "peer channel create -o localhost:7050 -c the-channel -f ../artefacts/channel_creation.tx  --outputBlock ../artefacts/the-channel.block"
		write text "docker-compose logs -f"
	end tell
	
	my change_directory(2, "alphapeer")
	my change_directory(3, "betapeer")
	my change_directory(4, "gammapeer")
	my change_directory(5, "bankpeer")
	
	my change_directory(6, "alphaadmin")
	my change_directory(7, "betaadmin")
	my change_directory(8, "gammaadmin")
	my change_directory(9, "bankadmin")
	
	delay 5
	
	repeat with sessionNumber in {2, 3, 4, 5}
		tell session sessionNumber of current tab of current window
			write text "peer node start --peer-chaincodedev=true"
		end tell
	end repeat
	
	delay 2
	
	repeat with sessionNumber in {6, 7, 8, 9}
		tell session sessionNumber of current tab of current window
			write text "peer channel join -b ../artefacts/the-channel.block"
		end tell
	end repeat
	
end tell

on change_directory(session_num, dir)
	tell application "iTerm"
		tell session session_num of current tab of current window
			write text ("cd $GOPATH/src/github.com/appliedblockchain/zktrading/js/packages/k0-integration-tests-fabric/devnetwork/" & dir)
		end tell
	end tell
end change_directory