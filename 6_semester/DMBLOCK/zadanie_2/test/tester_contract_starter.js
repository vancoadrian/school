const ContractStarter = artifacts.require('Battleship');
const BOARD_LEN = 6;

async function init_accounts() {
    var accounts = await web3.eth.getAccounts();
    player1_address = accounts[0];
    player2_address = accounts[1];
    random_noob = accounts[2];
}

init_accounts();

let board_player_1 = [
    [true, true, false, false, false, false],
    [true, true, false, false, false, false],
    [true, true, false, false, false, false],
    [true, true, false, false, false, false],
    [true, false, false, false, false, false],
    [true, false, false, false, false, false]
];

let board_player_2 = [
    [true, true, false, false, false, false],
    [true, true, false, false, false, false],
    [true, true, false, false, false, false],
    [true, true, false, false, false, false],
    [true, false, false, false, false, false],
    [true, false, false, false, false, false]
];

nonces_1 = [
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
];

nonces_2 = [
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
    [178213580, 2172447116, 3044044087, 1163846527, 3450335091, 1414977252],
];

async function get_proof_for_board_guess(initial_board, nonces, guess) {
    let merkle_tree = build_merkle(initial_board, nonces);
    let index_in_merkle = guess[0] * BOARD_LEN + guess[1];
    let proof = [];
    for (let i = 0; i < merkle_tree.length - 1; i++) {
        let merkle_group = Math.floor(index_in_merkle / Math.pow(2, i)); // goodod
        let index_in_group = merkle_group % 2;
        let sibling = Math.min(merkle_group - index_in_group + (index_in_group + 1) % 2, merkle_tree[i].length - 1);
        let group_index = (index_in_merkle - (merkle_group * Math.pow(2, i + 1)));
        if (sibling == merkle_group) continue;
        proof.push(merkle_tree[i][sibling]);
    }
    return proof;
}


async function build_board_commitment(initial_board, nonces) {
    let merkle_tree = await build_merkle(initial_board, nonces);
    // the root of the merkle tree will be the last level's only element
    return merkle_tree[merkle_tree.length - 1][0];
}

async function build_merkle(initial_board, nonces) {
    //check_correct_sizes(initial_board, nonces);
    let merkle = [[]];
    // add all leaf nodes
    for (let i = 0; i < BOARD_LEN; i++) {
        for (let j = 0; j < BOARD_LEN; j++) {
            //console.log(nonces[i][j]);

            var a = JSON.stringify(initial_board[i][j]);
            var b = JSON.stringify(nonces[i][j]);

            //console.log();
            merkle[0].push(
                web3.utils.keccak256(
                    web3.utils.fromAscii(a + b)
                )
            );
        }
    }
    // build tree from leaves
    // while, current level of merkle has length > 1, add more levels
    let curr_level = 0;
    while (merkle[curr_level].length > 1) {
        merkle.push([]);
        curr_level += 1;
        // build new layer of tree
        for (let i = 0; i + 1 < merkle[curr_level - 1].length; i += 2) {
            // have new_node represent another node in the Merkle tree
            let new_node = web3.utils.keccak256(merkle[curr_level - 1][i] + merkle[curr_level - 1][i + 1].substring(2), { encoding: 'hex' });
            // finalize finally computes the hash for every argument passed in update
            merkle[curr_level].push(new_node);
        }
        // if this most recent merkle level has an odd length, we need
        // to just hoist the last element into the next level
        if (merkle[curr_level - 1].length % 2 !== 0) {
            merkle[curr_level].push(merkle[curr_level - 1][merkle[curr_level - 1].length - 1]);
        }
    }
    return merkle;
}


contract('Battleship', () => {

    it('Should deploy smart contract properly', async () => {
        const Battleship = await ContractStarter.deployed();
        assert(Battleship.address !== '');
    });
    
    
    it('Should return false for is_game_over()', async () => {
        const Battleship = await ContractStarter.deployed();
        var game = await Battleship.is_game_over.call();
        assert.equal(game, false);
    });

    it('Should first player have setted bid', async () => {
        const Battleship = await ContractStarter.deployed();
        assert.equal(await Battleship.store_bid({ from: player1_address, value: 100000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , true);
    });


    it('Should not allow claim_win() for player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_win(player2_address, { from: player1_address, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow forfeit() for player gamestate', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.forfeit(player2_address, { from: player1_address, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow store_board_commitment() for player gamestate', async () => {
        const Battleship = await ContractStarter.deployed();

        const commit_1 = await build_board_commitment(board_player_1, nonces_1);

        assert.equal(await Battleship.store_board_commitment(commit_1, { from: player1_address, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow check_one_ship() for player gamestate', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        assert.equal(await Battleship.check_one_ship(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address, gas: 2000000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                //console.log(err);
                return false;
            })
            , false);
    });

    it('Should not allow first player set bid again', async () => {
        const Battleship = await ContractStarter.deployed();
        //player_1.place_bet(0.0000000000001);
        //await expect(Battleship.store_bid({ from: player1_address, value: 100000 })).to.be.rejectedWith(Error);
        
        assert.equal(await Battleship.store_bid({ from: player1_address, value: 100000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow second player set bid low value', async () => {
        const Battleship = await ContractStarter.deployed();
        //player_1.place_bet(0.0000000000001);
        //await expect(Battleship.store_bid({ from: player1_address, value: 100000 })).to.be.rejectedWith(Error);

        assert.equal(await Battleship.store_bid({ from: player2_address, value: 1000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should second player have setted bid', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.store_bid({ from: player2_address, value: 1000000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , true);
    });

    it('Should not accuse_cheating() end game for player gamestate', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        //await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address });
        assert.equal(await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address })
            .then((result) => {
                return true;
            }).catch((err) => {
                //console.log(err);
                return false;
            })
            , false);
        //assert.equal(await Battleship.is_game_over.call(), false);
    });

    it('Should not allow second player set bid again', async () => {
        const Battleship = await ContractStarter.deployed();
        //player_1.place_bet(0.0000000000001);
        //await expect(Battleship.store_bid({ from: player1_address, value: 100000 })).to.be.rejectedWith(Error);

        assert.equal(await Battleship.store_bid({ from: player2_address, value: 100000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    

    it('Should not allow claim_win() for first player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_win({ from: player1_address, value: 0})
        .then((result) => {
            return true;
        }).catch((err) => {
            return false;
        })
        , false);
    });

    it('Should not allow claim_win() for second player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_win({ from: player2_address, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    //=====================================
    //=====================================
    //=====================================
    //===============non_player============
    //=====================================
    //=====================================

    it('Should not allow store_bid() for non player as next player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.store_bid({ from: random_noob, value: 100000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow store_board_commitment() for non player', async () => {
        const Battleship = await ContractStarter.deployed();

        //using player_1 nonces and board just for testing 
        const commit_non_player = await build_board_commitment(board_player_1, nonces_1);

        assert.equal(await Battleship.store_board_commitment(commit_non_player,{ from: random_noob, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow check_one_ship() for non player', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        assert.equal(await Battleship.check_one_ship(opening_nonce, proof, guess_leaf_index, player2_address, { from: random_noob, gas: 2000000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                 //console.log(err);
                return false;
            })
            , false);
    });

    it('Should not allow claim_win() for non player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_win({ from: random_noob, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow forfeit() for non player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_win(player1_address,{ from: random_noob, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });


    it('Should not allow claim_opponent_left() for non player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_opponent_left(player1_address, { from: random_noob, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow handle_timeout() for non player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.handle_timeout(player1_address, { from: random_noob, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    it('Should not allow claim_timeout_winnings() for non player', async () => {
        const Battleship = await ContractStarter.deployed();

        assert.equal(await Battleship.claim_opponent_left(player1_address, { from: random_noob, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , false);
    });

    //=====================================
    //=====================================
    //=====================================

    it('Should allow store_board_commitment() for first player', async () => {
        const Battleship = await ContractStarter.deployed();


        const commit_1 = await build_board_commitment(board_player_1, nonces_1);

        assert.equal(await Battleship.store_board_commitment(commit_1, { from: player1_address, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , true);
    });


    it('Should allow store_board_commitment() for second player', async () => {
        const Battleship = await ContractStarter.deployed();


        const commit_2 = await build_board_commitment(board_player_2, nonces_2);

        assert.equal(await Battleship.store_board_commitment(commit_2, { from: player2_address, value: 0 })
            .then((result) => {
                return true;
            }).catch((err) => {
                return false;
            })
            , true);
    });


    it('Should be prevented claim_timeout_winnings() for both players', async () => {
        const Battleship = await ContractStarter.deployed();


        //const commit_2 = await build_board_commitment(board_player_2, nonces_2);
        await Battleship.claim_opponent_left(player2_address, { from: player1_address });
        await Battleship.claim_opponent_left(player1_address, { from: player2_address });
        await Battleship.handle_timeout(player1_address, { from: player2_address });
        await Battleship.handle_timeout(player2_address, { from: player1_address });
        await Battleship.claim_timeout_winnings(player2_address, { from: player1_address });
        await Battleship.claim_timeout_winnings(player1_address, { from: player2_address });

        assert.equal(await Battleship.is_game_over.call(), false);
    });

    it('Should allow check_one_ship() for player', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        assert.equal(await Battleship.check_one_ship(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address, gas: 2000000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                //console.log(err);
                return false;
            })
            , true);
    });

    it('Should allow check_one_ship() for player', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        assert.equal(await Battleship.check_one_ship(opening_nonce, proof, guess_leaf_index, player2_address, { from: player2_address, gas: 2000000 })
            .then((result) => {
                return true;
            }).catch((err) => {
                //console.log(err);
                return false;
            })
            , true);
    });


    it('Should not accuse_cheating() end game for non player', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        //await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address });
        assert.equal(await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: random_noob })
            .then((result) => {
                return true;
            }).catch((err) => {
               //console.log(err);
                return false;
            })
            , false);
        //assert.equal(await Battleship.is_game_over.call(), false);
    });


    it('Should return merged', async () => {
        const Battleship = await ContractStarter.deployed();
        const commit_1 = await build_board_commitment(board_player_1, nonces_1);
        const commit_2 = await build_board_commitment(board_player_2, nonces_2);
        assert.notEqual(await Battleship.merge_bytes32(commit_1, commit_2), commit_1 + commit_2);
    });

    it('Should not accuse_cheating() end game for player', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, j]);
        //await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address });
        assert.equal(await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player2_address })
            .then((result) => {
                return true;
            }).catch((err) => {
                //console.log(err);
                return false;
            })
            , false);
        //assert.equal(await Battleship.is_game_over.call(), false);
    });



    it('Should accuse_cheating() end game for player', async () => {
        const Battleship = await ContractStarter.deployed();
        var i = 0, j = 0;
        var opening_nonce = web3.utils.fromAscii(JSON.stringify(board_player_2[i][j]) + JSON.stringify(nonces_2[i][j]));
        var guess_leaf_index = i * BOARD_LEN + j;
        var proof = await get_proof_for_board_guess(board_player_2, nonces_2, [i, 5]);
        await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address });
        // assert.equal(await Battleship.accuse_cheating(opening_nonce, proof, guess_leaf_index, player2_address, { from: player1_address })
        //     .then((result) => {
        //         return true;
        //     }).catch((err) => {
        //         console.log(err);
        //         return false;
        //     })
        //     , true);
        assert.equal(await Battleship.is_game_over.call(), true);
    });
});

contract('Battleship', () => {

    it('Should first player won after second player forfeit', async () => {
        const Battleship = await ContractStarter.deployed();

        await Battleship.store_bid({ from: player1_address, value: 100000 });
        await Battleship.store_bid({ from: player2_address, value: 100000 });

        const commit_1 = await build_board_commitment(board_player_1, nonces_1);
        const commit_2 = await build_board_commitment(board_player_2, nonces_2);

        await Battleship.store_board_commitment(commit_1, { from: player1_address });
        await Battleship.store_board_commitment(commit_2, { from: player2_address});

        await Battleship.forfeit(player1_address,{ from: player2_address, value: 0 });

        var game = await Battleship.is_game_over.call();

        assert.equal(game, true);
    });
});


contract('Battleship', () => {

    it('Should second player won after first player forfeit', async () => {
        const Battleship = await ContractStarter.deployed();

        await Battleship.store_bid({ from: player1_address, value: 100000 });
        await Battleship.store_bid({ from: player2_address, value: 100000 });

        const commit_1 = await build_board_commitment(board_player_1, nonces_1);
        const commit_2 = await build_board_commitment(board_player_2, nonces_2);

        await Battleship.store_board_commitment(commit_1, { from: player1_address });
        await Battleship.store_board_commitment(commit_2, { from: player2_address });

        await Battleship.forfeit(player2_address, { from: player1_address, value: 0 });

        var game = await Battleship.is_game_over.call();

        assert.equal(game, true);
    });
});

contract('Battleship', () => {

    it('Should not allow forfeit for player because sending ourself as opponent', async () => {
        const Battleship = await ContractStarter.deployed();

        await Battleship.store_bid({ from: player1_address, value: 100000 });
        //bigger bid
        await Battleship.store_bid({ from: player2_address, value: 1000000 });

        const commit_1 = await build_board_commitment(board_player_1, nonces_1);
        const commit_2 = await build_board_commitment(board_player_2, nonces_2);

        await Battleship.store_board_commitment(commit_1, { from: player1_address });
        await Battleship.store_board_commitment(commit_2, { from: player2_address });

        await Battleship.forfeit(player1_address, { from: player1_address, value: 0 }).then((result) => {

        }).catch((err) => {
            //console.log(err);
        });

        var game = await Battleship.is_game_over.call();

        assert.equal(game, false);
    });

}); 
