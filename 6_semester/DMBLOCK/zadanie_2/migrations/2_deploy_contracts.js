var contract_starter = artifacts.require("Battleship");

module.exports = function (deployer) {
    deployer.deploy(contract_starter);
}