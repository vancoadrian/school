// SPDX-License-Identifier: MIT
pragma solidity >=0.4.22 <0.7.0;

import "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/release-v3.0.0/contracts/cryptography/ECDSA.sol";
//import "../ECDSA.sol";

contract Battleship {
    using ECDSA for bytes32;
    uint32 constant BOARD_LEN = 6;

    // Declare state variables here.
    bool player_1_accused_prevented = true;
    bool player_2_accused_prevented = true;
    bool first_player = true;
    bool board_p1_asigned = false;
    
    bytes32 board_p1;
    bytes32 board_p2;

    bool board_p2_asigned = false;

    enum GameStates { OPEN, PLACING, RUNNING, FINISHED, DONE }
    GameStates game_state = GameStates.OPEN;

    // Consider keeping state for:
    // - player addresses
    address payable player_1;
    address payable player_2;

    uint player_1_bid;  
    uint player_2_bid;
    //uint player_2_refund;

    // - whether the game is over
    // - board commitments
    
    // - whether a player has proven 10 winning moves
    
    uint player_1_left_start_time;
    uint player_2_left_start_time;

    bytes true_in_bytes = 'true';
    // - whether a player has proven their own board had 10 ships
    uint8[] player_1_ships;
    uint8[] player_2_ships;
    uint8[] player_1_waters;
    uint8[] player_2_waters;

    uint8 player_1_hits = 0;
    uint8 player_2_hits = 0;
    // Declare events here.
    // Consider triggering an event upon accusing another player of having left.
    event TimeAccusion(address plaintiff, address  accused, uint time_of_accusion);
    event CheckOneShip(address caller, address board_owner, bool telling_truth);
    //event IsGameOver(bool game_over);
    //event ClaimedTimeoutWin(address who);
    //event TimeDifference(uint difference);
    // Store the bids of each player
    // Start the game when both bids are received
    //uint game_start_time;

    //----------------------------------------------
    //----------------------------------------------
    //------------test_helping_functions------------
    //----------------------------------------------

    // function get_player_1_bid() public view returns (uint){
    //     return player_1_bid;
    // }

    // function get_player_2_bid() public view returns (uint){
    //     return player_2_bid;
    // }

    // function get_board_p1() public view returns (bytes32){
    //     return board_p1;
    // }

    // function get_board_p2() public view returns (bytes32){
    //     return board_p2;
    // }
    
    //----------------------------------------------
    //----------------------------------------------
    //----------------------------------------------



    
    // The first player to call the function determines the bid amount.
    // Refund excess bids to the second player if they bid too much.
    function store_bid() public payable{

        require(game_state == GameStates.OPEN || game_state == GameStates.DONE);

        if(game_state == GameStates.DONE){
            game_state = GameStates.OPEN;
        }

        if(first_player){
            // if (msg.value <= 0) {
            //     throw;
            // }
            first_player = false;
            player_1 = msg.sender;
            player_1_bid = msg.value;
        } 
        else{
            require(msg.sender != player_1);
            require(msg.value >= player_1_bid);
            // if(msg.value < player_1_bid){
            //     throw;
            // }
            if(msg.value > player_1_bid){
                //player_2_refund = msg.value - player_1_bid;
                //payable(msg.sender).transfer(player_2_refund);
                payable(msg.sender).transfer(msg.value - player_1_bid);
            }
            player_2 = msg.sender;
            //player_2_bid = msg.value - player_2_refund;
            player_2_bid = player_1_bid;
            //game start time
            //game_start_time = now;
            //change game state
            game_state = GameStates.PLACING;
            
        }
    }

    // Clear state - make sure to set that the game is not in session
    function clear_state() internal {
        
        require(game_state == GameStates.FINISHED);

        first_player = true; 

        player_1 = address(0);
        player_2 = address(0);

        player_1_bid = 0;
        player_2_bid = 0;

        player_1_accused_prevented = true;
        player_2_accused_prevented = true;

        board_p1_asigned = false;
        board_p2_asigned = false;

        board_p1 = "";
        board_p2 = "";

        delete player_1_ships;
        delete player_2_ships;
        delete player_1_waters;
        delete player_2_waters;

        player_1_hits = 0;
        player_2_hits = 0;

        game_state = GameStates.DONE;
    }

    // Store the initial board commitments of each player
    // Note that merkle_root is the hash of the topmost value of the merkle tree
    function store_board_commitment(bytes32 merkle_root) public {
        
        require(game_state == GameStates.PLACING);
        require(msg.sender == player_1 || msg.sender == player_2);

        if (msg.sender == player_1){
            board_p1 = merkle_root;
            board_p1_asigned = true;
        }
        else{
            board_p2 = merkle_root;
            board_p2_asigned = true;
        }

        if(board_p1_asigned && board_p2_asigned){
            game_state = GameStates.RUNNING;
        }
    }

    // Verify the placement of one ship on a board
    // opening_nonce - corresponds to web3.utils.fromAscii(JSON.stringify(opening) + JSON.stringify(nonce)) in JS
    // proof - a list of sha256 hashes you can get from get_proof_for_board_guess
    // guess_leaf_index - the index of the guess as a leaf in the merkle tree
    // owner - the address of the owner of the board on which this ship lives
    function check_one_ship(bytes memory opening_nonce, bytes32[] memory proof,
        uint256 guess_leaf_index, address owner) public returns (bool result) {
        
        require(game_state == GameStates.RUNNING);
        require(msg.sender == player_1 || msg.sender == player_2);
        require(owner == player_1 || owner == player_2);

        bool telling_truth;
        bool ship = true;

        if(owner == player_1){
            telling_truth = verify_opening(opening_nonce, proof,guess_leaf_index,board_p1);

            for(uint i = 0; i < 4; i++){
                if(true_in_bytes[i] != opening_nonce[i]){
                    ship = false;
                    break;
                }
            }

            if(ship && telling_truth || !ship && !telling_truth){
                for(uint i = 0; i < player_1_ships.length;i++){
                    if(player_1_ships[i] == guess_leaf_index){
                        emit CheckOneShip(msg.sender, owner, telling_truth);
                        return telling_truth;
                    }
                }

                if(msg.sender != owner){
                   player_2_hits++; 
                } 

                player_1_ships.push(uint8(guess_leaf_index));
            }
            else{
                for(uint i = 0; i < player_1_waters.length;i++){
                    if(player_1_waters[i] == guess_leaf_index){
                        emit CheckOneShip(msg.sender, owner, telling_truth);
                        return telling_truth;
                    }
                }
                player_1_waters.push(uint8(guess_leaf_index));
            }
            emit CheckOneShip(msg.sender, owner, telling_truth);
            return telling_truth;
        }
        else{
            telling_truth = verify_opening(opening_nonce, proof,guess_leaf_index,board_p2);

            for(uint i = 0; i < 4; i++){
                if(true_in_bytes[i] != opening_nonce[i]){
                    ship = false;
                    break;
                }
            }

            if(ship && telling_truth || !ship && !telling_truth){
                for(uint i = 0; i < player_2_ships.length;i++){
                    if(player_2_ships[i] == guess_leaf_index){
                        emit CheckOneShip(msg.sender, owner, telling_truth);
                        return telling_truth;
                    }
                }

                if(msg.sender != owner){
                player_1_hits++;
                }

                player_2_ships.push(uint8(guess_leaf_index));

            }
            else{
                for(uint i = 0; i < player_2_waters.length;i++){
                    if(player_2_waters[i] == guess_leaf_index){
                        emit CheckOneShip(msg.sender, owner, telling_truth);
                        return telling_truth;
                    }
                }
                player_2_waters.push(uint8(guess_leaf_index));
            }
            emit CheckOneShip(msg.sender, owner, telling_truth);
            return telling_truth;
        }

    }

    // Claim you won the game
    // If you have checked 10 winning moves (hits) AND you have checked
    // 10 of your own ship placements with the contract, then this function
    // should transfer winning funds to you and end the game.
    function claim_win() public {
        
        require(msg.sender == player_1 && player_1_hits == 10 && player_1_ships.length == 10 || 
        msg.sender == player_1 && player_2_ships.length != 10 && player_2_ships.length + player_2_waters.length == BOARD_LEN*BOARD_LEN || 
        msg.sender == player_2 && player_2_hits == 10 && player_2_ships.length == 10|| 
        msg.sender == player_2 && player_1_ships.length != 10 && player_1_ships.length + player_1_waters.length == BOARD_LEN*BOARD_LEN);

        //change game state
        game_state = GameStates.FINISHED;
        
        uint amount = player_1_bid + player_2_bid;

        player_1_bid = 0;
        player_2_bid = 0;

        msg.sender.transfer(amount);

        clear_state();
    }

    // Forfeit the game
    // Regardless of cheating, board state, or any other conditions, this function
    // results in all funds being sent to the opponent and the game being over.
    function forfeit(address payable opponent) public {
        
        require(game_state == GameStates.RUNNING);
        require(msg.sender == player_1 && opponent == player_2 || msg.sender == player_2 && opponent == player_1);

        //change game state
        game_state = GameStates.FINISHED;

        uint amount = player_1_bid + player_2_bid;

        player_1_bid = 0;
        player_2_bid = 0;

        opponent.transfer(amount);

        clear_state();
    }

    // Claim the opponent cheated - if true, you win.
    // opening_nonce - corresponds to web3.utils.fromAscii(JSON.stringify(opening) + JSON.stringify(nonce)) in JS
    // proof - a list of sha256 hashes you can get from get_proof_for_board_guess (this is what the sender believes to be a lie)
    // guess_leaf_index - the index of the guess as a leaf in the merkle tree
    // owner - the address of the owner of the board on which this ship lives
    function accuse_cheating(bytes memory opening_nonce, bytes32[] memory proof,
        uint256 guess_leaf_index, address owner) public returns (bool result) {

        require(game_state == GameStates.RUNNING);
        require(msg.sender == player_1 && owner == player_2 || msg.sender == player_2 && owner == player_1);

        bool telling_truth;
        if(owner == player_1){
            telling_truth = verify_opening(opening_nonce, proof,guess_leaf_index,board_p1);
            if(!telling_truth){
                //change game state
                game_state = GameStates.FINISHED;
                
                uint amount = player_1_bid + player_2_bid;

                player_1_bid = 0;
                player_2_bid = 0;


                msg.sender.transfer(amount);

                clear_state();
            }

            return telling_truth;
        }
        else{
            telling_truth = verify_opening(opening_nonce, proof,guess_leaf_index,board_p2);
            if(!telling_truth){
                //change game state
                game_state = GameStates.FINISHED;
                
                uint amount = player_1_bid + player_2_bid;

                player_1_bid = 0;
                player_2_bid = 0;

                msg.sender.transfer(amount);

                clear_state();

            }

            return telling_truth;
        }

    }

    // Claim the opponent of taking too long/leaving
    // Trigger an event that both players should listen for.
    function claim_opponent_left(address opponent) public {

        require(msg.sender == player_1 && opponent == player_2 || msg.sender == player_2 && opponent == player_1);
        
        if (msg.sender == player_1){
            player_2_left_start_time = now;
            player_2_accused_prevented = false;
            emit TimeAccusion(msg.sender, player_2, now);
        }
        else{
            player_1_left_start_time = now;
            player_1_accused_prevented = false;
            emit TimeAccusion(msg.sender, player_1, now);
        }
    }

    // Handle a timeout accusation - msg.sender is the accused party.
    // If less than 1 minute has passed, then set state appropriately to prevent distribution of winnings.
    // Otherwise, do nothing.
    function handle_timeout(address payable opponent) public {
        
        require(msg.sender == player_1 && opponent == player_2 || msg.sender == player_2 && opponent == player_1);

        if(msg.sender == player_1){
            if(now - player_1_left_start_time < 60){
                player_1_accused_prevented = true;
            }
            //emit TimeDifference(now - player_1_left_start_time);
        }
        else {
            if(now - player_2_left_start_time < 60){
                player_2_accused_prevented = true;
            }
            //emit TimeDifference(now - player_1_left_start_time);
        }
    }

    // Claim winnings if opponent took too long/stopped responding after claim_opponent_left
    // The player MUST claim winnings. The opponent failing to handle the timeout on their end should not
    // result in the game being over. If the timer has not run out, do nothing.
    function claim_timeout_winnings(address opponent) public {
        
        require(msg.sender == player_1 && opponent == player_2 || msg.sender == player_2 && opponent == player_1);

        if(msg.sender == player_1){
            if(!player_2_accused_prevented && now - player_2_left_start_time >= 60){
                
                //change game state
                game_state = GameStates.FINISHED;
                
                uint amount = player_1_bid + player_2_bid;

                player_1_bid = 0;
                player_2_bid = 0;

                msg.sender.transfer(amount);

                clear_state();

                //emit ClaimedTimeoutWin(msg.sender);
            }
        }
        else {
            if(!player_1_accused_prevented && now - player_1_left_start_time >= 60){
                
                //change game state
                game_state = GameStates.FINISHED;
                
                uint amount = player_1_bid + player_2_bid;

                player_1_bid = 0;
                player_2_bid = 0;

                msg.sender.transfer(amount);

                clear_state();

                //emit ClaimedTimeoutWin(msg.sender);
            }
        }
    }

    // Check if game is over
    // Hint - use a state variable for this, so you can call it from JS.
    // Note - you cannot use the return values of functions that change state in JS.
    function is_game_over() public returns (bool) {
        
        if(game_state == GameStates.DONE){
            //emit IsGameOver(true);
            return true;
        }
        else{
            //emit IsGameOver(false);
            return false;
        }
    }

    /**** Helper Functions below this point. Do not modify. ****/
    /***********************************************************/

    function merge_bytes32(bytes32 a, bytes32 b) pure public returns (bytes memory) {
        bytes memory result = new bytes(64);
        assembly {
            mstore(add(result, 32), a)
            mstore(add(result, 64), b)
        }
        return result;
    }

    // Verify the proof of a single spot on a single board
    // \args:
    //      opening_nonce - corresponds to web3.utils.fromAscii(JSON.stringify(opening) + JSON.stringify(nonce)));
    //      proof - list of sha256 hashes that correspond to output from get_proof_for_board_guess()
    //      guess - [i, j] - guess that opening corresponds to
    //      commit - merkle root of the board
    function verify_opening(bytes memory opening_nonce, bytes32[] memory proof, uint guess_leaf_index, bytes32 commit) public view returns (bool result) {
        bytes32 curr_commit = keccak256(opening_nonce); // see if this changes hash
        uint index_in_leaves = guess_leaf_index;

        uint curr_proof_index = 0;
        uint i = 0;

        while (curr_proof_index < proof.length) {
            // index of which group the guess is in for the current level of Merkle tree
            // (equivalent to index of parent in next level of Merkle tree)
            uint group_in_level_of_merkle = index_in_leaves / (2**i);
            // index in Merkle group in (0, 1)
            uint index_in_group = group_in_level_of_merkle % 2;
            // max node index for currrent Merkle level
            uint max_node_index = ((BOARD_LEN * BOARD_LEN + (2**i) - 1) / (2**i)) - 1;
            // index of sibling of curr_commit
            uint sibling = group_in_level_of_merkle - index_in_group + (index_in_group + 1) % 2;
            i++;
            if (sibling > max_node_index) continue;
            if (index_in_group % 2 == 0) {
                curr_commit = keccak256(merge_bytes32(curr_commit, proof[curr_proof_index]));
                curr_proof_index++;
            } else {
                curr_commit = keccak256(merge_bytes32(proof[curr_proof_index], curr_commit));
                curr_proof_index++;
            }
        }
        return (curr_commit == commit);

    }
}
