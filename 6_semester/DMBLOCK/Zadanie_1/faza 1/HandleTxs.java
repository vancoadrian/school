import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

// Meno študenta: Adrián Vančo
public class HandleTxs {

    private UTXOPool utxoPool;

    /**
     * Vytvorí verejný ledger, ktorého aktuálny UTXOPool (zbierka nevyčerpaných
     * transakčných výstupov) je {@code utxoPool}. Malo by to vytvoriť obchrannú kópiu
     * utxoPool pomocou konštruktora UTXOPool (UTXOPool uPool).
     */
    public HandleTxs(UTXOPool utxoPool) {
        this.utxoPool = new UTXOPool(utxoPool);
    }


    /**
     * @return true, ak 
     * (1) sú všetky výstupy nárokované {@code tx} v aktuálnom UTXO pool, 
     * (2) podpisy na každom vstupe {@code tx} sú platné, 
     * (3) žiadne UTXO nie je nárokované viackrát, 
     * (4) všetky výstupné hodnoty {@code tx}s sú nezáporné a 
     * (5) súčet vstupných hodnôt {@code tx}s je väčší alebo rovný súčtu jej
     *     výstupných hodnôt; a false inak.
     */
    public boolean txIsValid(Transaction tx) {
        //ziskanie inputov z transakcie
        ArrayList<Transaction.Input> txInputs = tx.getInputs();
        double valueInputs = 0;
        //ziskanie outputov z transakcie
        ArrayList<Transaction.Output> txOutputs = tx.getOutputs();
        double valueOutputs = 0;

        HashSet<UTXO> UTXOsToBeSpended = new HashSet<UTXO>();
        Transaction.Output utxoOutput;

        //index inputu v transakcii
        int inputIndex = 0;

        for (Transaction.Input input : txInputs){
            UTXO utxoOfInput = new UTXO(input.prevTxHash, input.outputIndex);
            //(1)
            //ak nemina output v utxopoole
            if (!this.utxoPool.contains(utxoOfInput)){
                return false;
            }
            //(2)
            //ziskanie utxo pre porovnanie podpisov
            utxoOutput = this.utxoPool.getTxOutput(utxoOfInput);
            //ak nesedi podpis znamena to ze utxo odosielatel nevlastni
            if (!utxoOutput.address.verifySignature(tx.getDataToSign(inputIndex),input.signature)){
                return false;
            }
            //(3)
            //ak chce input vyuzit utxo, ktore uz bolo vyuzite (double-spending kontrola)
            if (UTXOsToBeSpended.contains(utxoOfInput)){
                return false;
            }
            //pridatie UTXO, ktore bude spendnute
            UTXOsToBeSpended.add(utxoOfInput);

            //naratavanie hodnoty vstupov
            valueInputs += utxoOutput.value;

            inputIndex++;
        }


        //naratanie hodnoty vystupov
        for (Transaction.Output output : txOutputs){
            //(4)
            if (output.value < 0){
                return false;
            }
            valueOutputs += output.value;
        }

        //(5)
        return !(valueInputs < valueOutputs);
    }

    /**
     * Spracováva každú epochu prijímaním neusporiadaného radu navrhovaných
     * transakcií, kontroluje správnosť každej transakcie, vracia pole vzájomne 
     * platných prijatých transakcií a aktualizuje aktuálny UTXO pool podľa potreby.
     */
    public Transaction[] handler(Transaction[] possibleTxs) {
        //obsahuje transakcie, ktoré sa schvalili
        ArrayList<Transaction> validTransactions  = new ArrayList<Transaction>();
        //obsahuje transakcie, ktore sa maju schvalit
        ArrayList<Transaction> obtainedTransactions = new ArrayList<Transaction>(Arrays.asList(possibleTxs));
        ArrayList<Transaction.Input> txInputs;
        ArrayList<Transaction.Output> txOutputs;
        //index outputu v urcitej transakcii
        int outputIndex;
        //pre while cyklus
        boolean wasAtLeastOneValid = true;

        //while cyklus pre transakcie, ktoré sa odkazujú na transakciu v rovnakom bloku
        /*
        ak bola aspoň nejaka transakcia valídna znamená to že sa pridali UTXOs z tejto validnej transakcie a treba prejsť
        ešte raz transakcie ktoré sa majú overiť
         */
        while (wasAtLeastOneValid){
            //obsahuje transakcie ktore prešli v cykle a mozu sa odobrat z ArrayList čakajúcich na schválenie
            ArrayList<Transaction> toBeRemovedTransactions = new ArrayList<Transaction>();
            wasAtLeastOneValid = false;

            for (Transaction transaction : obtainedTransactions){
                if (!txIsValid(transaction)){
                    continue;
                }

                if (!wasAtLeastOneValid){
                    wasAtLeastOneValid = true;
                }

                validTransactions.add(transaction);
                toBeRemovedTransactions.add(transaction);

                //update utxoPool
                txInputs = transaction.getInputs();
                txOutputs = transaction.getOutputs();
                //remove used utxo
                for (Transaction.Input input : txInputs){
                    UTXO utxoOfInput = new UTXO(input.prevTxHash, input.outputIndex);
                    this.utxoPool.removeUTXO(utxoOfInput);
                }
                //add new utxo from transaction
                outputIndex = 0;
                for (Transaction.Output output : txOutputs){
                    UTXO utxoOfOutput = new UTXO(transaction.getHash(),outputIndex);
                    utxoPool.addUTXO(utxoOfOutput,output);

                    outputIndex++;
                }
            }
            //odobratie transakcií, ktoré už prešli
            for (Transaction transaction : toBeRemovedTransactions){
                obtainedTransactions.remove(transaction);
            }
        }

        //prehodenie do Array
        Transaction[] toBeReturned = new Transaction[validTransactions.size()];
        toBeReturned = validTransactions.toArray(toBeReturned);

        return toBeReturned;
    }
}
