tell application "System Events" to keystroke "t" using command down
tell application "iTerm"
	activate
	select first window
	create tab with default profile
	
	tell current session of current window
		write text "cd $GOPATH/src/github.com/appliedblockchain/zktrading/js/test/fabric/devnetwork"
		write text "cd $GOPATH/src/github.com/appliedblockchain/zktrading/go/chaincode/cash"
		write text "go build"
		delay 2
		split horizontally with default profile
		split horizontally with default profile
	end tell
	
	repeat with sessionNumber in {1, 5}
		tell session sessionNumber of current tab of current window
			split vertically with default profile
			split vertically with default profile
			split vertically with default profile
		end tell
	end repeat
	
	my change_directory(1, "js/test/fabric/devnetwork/alphaadmin")
	my change_directory(2, "js/test/fabric/devnetwork/betaadmin")
	my change_directory(3, "js/test/fabric/devnetwork/gammaadmin")
	my change_directory(4, "js/test/fabric/devnetwork/bankadmin")
	
	my change_directory(5, "go/chaincode/cash")
	my change_directory(6, "go/chaincode/cash")
	my change_directory(7, "go/chaincode/cash")
	my change_directory(8, "go/chaincode/cash")
	
	set chaincodeID to ""
	repeat 8 times
		set chaincodeID to chaincodeID & some item of "abcdefghijklmnopqrstuvwxyz"
	end repeat
	
	tell session 5 of current tab of current window
		write text "VERIFIER_ENDPOINT=http://localhost:11400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:11752 CORE_CHAINCODE_ID_NAME=" & chaincodeID & ":1 ./cash"
	end tell
	tell session 6 of current tab of current window
		write text "VERIFIER_ENDPOINT=http://localhost:12400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:12752 CORE_CHAINCODE_ID_NAME=" & chaincodeID & ":1 ./cash"
	end tell
	tell session 7 of current tab of current window
		write text "VERIFIER_ENDPOINT=http://localhost:13400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:13752 CORE_CHAINCODE_ID_NAME=" & chaincodeID & ":1 ./cash"
	end tell
	tell session 8 of current tab of current window
		write text "VERIFIER_ENDPOINT=http://localhost:14400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:14752 CORE_CHAINCODE_ID_NAME=" & chaincodeID & ":1 ./cash"
	end tell
	
	repeat with sessionNumber in {1, 2, 3, 4}
		tell session sessionNumber of current tab of current window
			write text "CORE_CHAINCODE_MODE=net peer chaincode install -p github.com/appliedblockchain/zktrading/go/chaincode/cash -n" & chaincodeID & " -v 1"
		end tell
	end repeat
	
	tell session 9 of current tab of current window
		write text "cd $GOPATH/src/github.com/appliedblockchain/zktrading/js/test/fabric"
		write text "export CHAINCODE_ID=" & chaincodeID
		write text "export DEV_MODE=true"
		write text "echo $CHAINCODE_ID"
		delay 5
		write text "node instantiate && mocha test.js; ./devnetwork/killChaincodes.sh"
	end tell
	
end tell

on change_directory(session_num, dir)
	tell application "iTerm"
		tell session session_num of current tab of current window
			write text "cd $GOPATH/src/github.com/appliedblockchain/zktrading/" & dir
		end tell
	end tell
end change_directory
