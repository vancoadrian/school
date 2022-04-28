using System.Net.NetworkInformation;

namespace Switch
{
    public class CamRecord
    {
        public int timer { get; set; }
        public Port port { get; set; }
        public PhysicalAddress macAddress { get; set; }

        public CamRecord(int time, Port port, PhysicalAddress address)
        {
            this.timer = time;
            this.port = port;
            this.macAddress = address; 
        }
    }
}
