using System.Collections.Generic;
using System.Timers;


namespace Switch
{
    public class CamTable
    {
        public List<CamRecord> camRecords { get; set; }
        public List<CamRecord> camRecordsToBeRemoved { get; set; }
        public Timer timer;
        private Switch SW;
        public int keepTime;



        public CamTable(Switch SW) {
            this.SW = SW;
            this.camRecords = new List<CamRecord>();
            this.camRecordsToBeRemoved = new List<CamRecord>();
            this.keepTime = 60;
            this.timer = new Timer(1000);
            this.timer.Elapsed += updateTimers;
            this.timer.AutoReset = true;
            this.timer.Enabled = true;
        }

        private void updateTimers(object state, ElapsedEventArgs e)
        {
            //throw new NotImplementedException();
            //Console.WriteLine("CAM updater ...running now");


            //update every record timer
            foreach (var item in this.camRecords)
            {
                //decrement timer of record
                item.timer--;

                //timer ended
                if (item.timer <= 0)
                {
                    //add to list for removing
                    camRecordsToBeRemoved.Add(item);
                    //Console.WriteLine("Removing"+item.macAddress);
                    continue;
                }
            }

            //delete records with timer value zero
            foreach(var item in camRecordsToBeRemoved)
            {
                camRecords.Remove(item);
            }

            //clear list
            camRecordsToBeRemoved.Clear();

            //volanie na vykreslovanie mac tabulky vo form
            SW.update_CAM_table(this.camRecords);


        }
    }
}
