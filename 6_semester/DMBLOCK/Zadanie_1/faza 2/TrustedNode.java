// Meno študenta: Adrián Vančo
import java.util.*;
import java.util.stream.Collectors;

/* TrustedNode označuje uzol, ktorý dodržuje pravidlá (nie je byzantský) */
public class TrustedNode implements Node {

    int rounds;
    Set<Transaction> pendingTransactions = null;
    boolean[] followees;
    HashMap<Transaction,Integer> howManyTrustOnTransaction;

    public TrustedNode(double p_graph, double p_byzantine, double p_txDistribution, int numRounds) {
        this.rounds = numRounds;
        this.howManyTrustOnTransaction = new HashMap<Transaction,Integer>();
    }

    public void followeesSet(boolean[] followees) {
        //tam kde je true toho sledujem
        this.followees = followees;
    }

    public void pendingTransactionSet(Set<Transaction> pendingTransactions) {
        //ulozim si transakcie, ktore došli
        this.pendingTransactions = pendingTransactions;

    }

    public Set<Transaction> followersSend() {
        //vraciam mnozinu transakcii, ktore su validne navzájom
        //asi všetky kedze su vsetky momentalne validne
        //znizujem tu rounds
        if (this.rounds == 0){
            //vrat transakcie ktorym doveruje najviac susedov
            try {
                int maxValue = Collections.max(this.howManyTrustOnTransaction.values());
                //System.out.println("MAX VALUE " +maxValue);
                Map<Transaction,Integer> tu = this.howManyTrustOnTransaction.entrySet().stream()
                        .filter(e-> e.getValue() == maxValue)
                        .collect(Collectors.toMap(e->e.getKey(),e->e.getValue()));
                //System.out.println("POCET KTORE MAJU MAX VALUE A IDU VON " + tu.keySet().size());
                return tu.keySet();
            }
            catch (Exception e) {
                System.out.println("Exception thrown : " + e);
            }
        }
        this.rounds--;
        return this.pendingTransactions;
    }

    public void followeesReceive(ArrayList<Integer[]> candidates) {
        //System.out.println("---------------------------------------------------------");
        this.howManyTrustOnTransaction = new HashMap<Transaction,Integer>();
        for (Integer[] candidat : candidates){
            int txId = candidat[0];
            int watched = candidat[1];

            if (!followees[watched]){
                continue;
            }

            Transaction obtainedTx = new Transaction(txId);
            if (!pendingTransactions.contains(obtainedTx)){
                this.pendingTransactions.add(obtainedTx);
            }
            if (!this.howManyTrustOnTransaction.containsKey(obtainedTx)){
                //nastav transakciu (doveruje mu jeden sused)
                this.howManyTrustOnTransaction.put(obtainedTx,1);
            } else {
                //ikrementuj pocet susedov, ktory maju tuto transakciu
              this.howManyTrustOnTransaction.put(obtainedTx, this.howManyTrustOnTransaction.get(obtainedTx) + 1);
            }

            //System.out.println("Lenght: " + candidat.length + ", [0]: " + candidat[0] +", [1]: " + candidat[1]);
            //System.out.println("Lenght: " + candidat.length + ", [0]: " + candidat[0].toString() +", [1]: " + candidat[1].toString());
        }
    }
}
