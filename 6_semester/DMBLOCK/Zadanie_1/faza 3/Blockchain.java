// Meno študenta: Adrián Vančo
// Blockchain by mal na uspokojenie funkcií udržiavať iba obmedzené množstvo uzlov
// Nemali by ste mať všetky bloky pridané do blockchainu v pamäti  
// pretože by to spôsobilo pretečenie pamäte.
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class Blockchain {
    public static final int CUT_OFF_AGE = 12;
    private int lastHeight;
    private BlockNode oldestMaxHeightBlock;
    private TransactionPool txPool;
    private HashMap<ByteArrayWrapper,BlockNode> H;

    // všetky potrebné informácie na spracovanie bloku v reťazi blokov
    private class BlockNode implements Comparable<BlockNode>{
        public Block b;
        public BlockNode parent;
        public ArrayList<BlockNode> children;
        public int height;
        // utxo pool na vytvorenie nového bloku na vrchu tohto bloku
        private UTXOPool uPool;

        public BlockNode(Block b, BlockNode parent, UTXOPool uPool) {
            this.b = b;
            this.parent = parent;
            children = new ArrayList<BlockNode>();
            this.uPool = uPool;
            if (parent != null) {
                height = parent.height + 1;
                parent.children.add(this);
            } else {
                height = 1;
            }
        }
        public UTXOPool getUTXOPoolCopy() {
            return new UTXOPool(uPool);
         }

        @Override
        public int compareTo(BlockNode o) {
            if (this.height > o.height) {
                return 1;
            } else if (this.height < o.height) {
                return -1;
            }
            return 0;
        }
    }
    /**
     * vytvor prázdny blockchain iba s Genesis blokom. Predpokladajme, že
     * {@code genesisBlock} je platný blok
     */
    public Blockchain(Block genesisBlock) {
        //vytvorenie UTXO z coinbase transakcie
        Transaction coinbase = genesisBlock.getCoinbase();
        Transaction.Output coinbaseOutput = coinbase.getOutput(0);
        UTXO coinbaseUTXO = new UTXO(coinbase.getHash(),0);

        //vytvorenie UTXOPoolu pre genesisBlock
        UTXOPool genBlockUTXOPool = new UTXOPool();
        genBlockUTXOPool.addUTXO(coinbaseUTXO, coinbaseOutput);

        this.oldestMaxHeightBlock = new BlockNode(genesisBlock, null, genBlockUTXOPool);
        this.lastHeight = this.oldestMaxHeightBlock.height;
        this.txPool = new TransactionPool();

        this.H = new HashMap<ByteArrayWrapper,BlockNode>();
        ByteArrayWrapper hash = new ByteArrayWrapper(genesisBlock.getHash());
        H.put(hash,this.oldestMaxHeightBlock);
    }

    /** Získaj maximum height blok */
    public Block getBlockAtMaxHeight() {
        return this.oldestMaxHeightBlock.b;
    }

    /** Získaj UTXOPool na ťaženie nového bloku na vrchu max height blok */
    public UTXOPool getUTXOPoolAtMaxHeight() {
        return this.oldestMaxHeightBlock.getUTXOPoolCopy();
    }

    /** Získaj pool transakcií na vyťaženie nového bloku */
    public TransactionPool getTransactionPool() {
        return new TransactionPool(this.txPool);
    }

    /**
     * Pridaj {@code block} do blockchainu, ak je platný. Kvôli platnosti by mali
     * byť všetky transakcie platné a blok by mal byť na
     * {@code height > (maxHeight - CUT_OFF_AGE)}.
     *
     * Môžete napríklad vyskúšať vytvoriť nový blok nad blokom Genesis (výška bloku
     * 2), ak height blockchainu je {@code <=
     * CUT_OFF_AGE + 1}. Len čo {@code height > CUT_OFF_AGE + 1}, nemôžete vytvoriť
     * nový blok vo výške 2.
     *
     * @return true, ak je blok úspešne pridaný
     */
    public boolean blockAdd(Block block) {

        if (block == null){
            return false;
        }

        //kontrola či už existuje podobný
        if (block.getHash() == null){
            return false;
        }

        ByteArrayWrapper hash = new ByteArrayWrapper(block.getHash());

        //kontrola či sa už nachádza v Blockchaine
        if (H.containsKey(hash)){
            return false;
        }

        //ziskanie parenta
        BlockNode parent = getBlockNode(block.getPrevBlockHash());

        //blok ukazuje na parenta, ktory v blockchaine neexistuje
        if (parent == null){
            return false;
        }

        HandleTxs validator = new HandleTxs(parent.getUTXOPoolCopy());

        //prehodenie do Array
        ArrayList<Transaction> transactionsArrayList = block.getTransactions();
        Transaction[] transactionsArray = new Transaction[transactionsArrayList.size()];
        transactionsArray = transactionsArrayList.toArray(transactionsArray);

        //ziskanie validnych transakcii a zaroven vo validatore je updatnuty utxoPool parenta
        Transaction[] validTransactions = validator.handler(transactionsArray);

        //blok nemal vsetky transakcie validne
        if (validTransactions.length != transactionsArray.length){
            return false;
        }

        //new utxo pool ==  updated parent utxopool  a utxopool of new outputs
        //asign new utxo pool
        UTXOPool utxoPoolNew = validator.UTXOPoolGet();

        //remove transactions
        for (Transaction transaction : transactionsArrayList){
            //remove transaction from txpool
            txPool.removeTransaction(transaction.getHash());
        }

        //add coinbase transaction to utxoPoolNew
        Transaction coinbase = block.getCoinbase();
        Transaction.Output coinbaseOutput = coinbase.getOutput(0);
        UTXO coinbaseUTXO = new UTXO(coinbase.getHash(),0);
        utxoPoolNew.addUTXO(coinbaseUTXO,coinbaseOutput);

        //vytvorenie BlockNode pre pridavany blok
        BlockNode blockNodeToBeAdded = new BlockNode(block,parent,utxoPoolNew);

        //pre odstranenie blokov kvoli CUT_OFF_AGE
        ArrayList<ByteArrayWrapper> blockNodesToBeRemoved = new ArrayList<ByteArrayWrapper>();

        //oseknutie
        if (blockNodeToBeAdded.height - this.lastHeight >= CUT_OFF_AGE){
            for (Map.Entry<ByteArrayWrapper,BlockNode> entry : this.H.entrySet()){
                if (entry.getValue().height <= this.lastHeight){
                    blockNodesToBeRemoved.add(entry.getKey());
                }
            }

            //odobratie referencii
            for (ByteArrayWrapper key : blockNodesToBeRemoved){
                BlockNode removedNode = H.remove(key);
                ArrayList<BlockNode> childrenOfRemovedNode = removedNode.children;
                for (int index = 0; index < childrenOfRemovedNode.size(); index++){
                    childrenOfRemovedNode.get(index).parent = null;
                }
            }

            this.lastHeight++;
        }

        //update referencie na najvyšieho
        if (blockNodeToBeAdded.height > oldestMaxHeightBlock.height){
            oldestMaxHeightBlock = blockNodeToBeAdded;
        }

        //add to blockchain
        H.put(hash,blockNodeToBeAdded);
        parent.children.add(blockNodeToBeAdded);


        return true;
    }

    /** Ziskaj požadovaný BlockNode podla Block hash z Blockchainu*/
    private BlockNode getBlockNode(byte[] blockHash){
        ByteArrayWrapper hash = new ByteArrayWrapper(blockHash);
        return H.get(hash);
    }


    /** Pridaj transakciu do transakčného poolu */
    public void transactionAdd(Transaction tx) {
        //kontrola double-spending a valueInputs < valueOutputs ???
        this.txPool.addTransaction(tx);
    }
}