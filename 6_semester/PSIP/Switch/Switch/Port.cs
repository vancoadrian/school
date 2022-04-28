using System;
using System.Collections.Generic;
using System.Timers;
using SharpPcap;
using SharpPcap.LibPcap;

namespace Switch
{
    public class Port
    {
        private Switch SW;
        private Dictionary<string, int> dict_to_update_in;
        private Dictionary<string, int> dict_to_update_out;

        public LibPcapLiveDevice thisDevice { get; set; }
        public HashSet<string> thisQueue;

        public LibPcapLiveDevice destDevice { get; set; }
        public HashSet<string> destQueue;

        private Timer timer;    //timer pre detekciu vytiahnutia kabla
        private int keepTime = 50000; //milisec

        public Port(Switch SW)
        {
            this.SW = SW;
            this.thisQueue = new HashSet<string>();
            this.dict_to_update_in = new Dictionary<string, int>()
            {
                { "ETH",0},
                { "ARP",0},
                { "IP",0},
                { "TCP",0},
                { "UDP",0},
                { "ICMP",0},
                { "HTTP",0},
                { "HTTPS",0},
                { "TOTAL",0}
            };
            this.dict_to_update_out = new Dictionary<string, int>()
            {
                { "ETH",0},
                { "ARP",0},
                { "IP",0},
                { "TCP",0},
                { "UDP",0},
                { "ICMP",0},
                { "HTTP",0},
                { "HTTPS",0},
                { "TOTAL",0}
            };

            this.timer = new Timer(keepTime);
            this.timer.Elapsed += clearCamTablePortRecords;
            this.timer.AutoReset = true;
            

        }

        public void clearCamTablePortRecords(object sender, ElapsedEventArgs e)
        {
            //TODO: iba pre ten port
            this.SW.camTable.camRecords.RemoveAll(s => s.port == this);
        }


        public void clearStats()
        {
            this.dict_to_update_in = new Dictionary<string, int>()
            {
                { "ETH",0},
                { "ARP",0},
                { "IP",0},
                { "TCP",0},
                { "UDP",0},
                { "ICMP",0},
                { "HTTP",0},
                { "HTTPS",0},
                { "TOTAL",0}
            }; 
            this.dict_to_update_out = new Dictionary<string, int>()
            {
                { "ETH",0},
                { "ARP",0},
                { "IP",0},
                { "TCP",0},
                { "UDP",0},
                { "ICMP",0},
                { "HTTP",0},
                { "HTTPS",0},
                { "TOTAL",0}
            };
        }


        public void listen()
        {
            var device = this.thisDevice;

            // Register our handler function to the 'packet arrival' event
            device.OnPacketArrival += new PacketArrivalEventHandler(device_OnPacketArrival);

            // Open the device for capturing
            int readTimeoutMilliseconds = 10;
            device.Open(mode: DeviceModes.Promiscuous | DeviceModes.DataTransferUdp | DeviceModes.NoCaptureLocal, read_timeout: readTimeoutMilliseconds);

            // Start the capturing process
            this.timer.Enabled = true;
            device.StartCapture();
        }


        public void stop_listen()
        {
            //this.thisDevice.StopCapture();
            //Thread.Sleep(1);
            this.thisDevice.Close();
            //Thread.Sleep(1);
            Console.WriteLine("Stoping Capture and closing device");
        }

        public void Reset(Timer timer)
        {
            timer.Stop();
            timer.Start();
        }


        public void device_OnPacketArrival(object sender, PacketCapture e)
        {

            var time = e.Header.Timeval.Date;
            var len = e.Data.Length;
            var rawPacket = e.GetPacket();

            var packet = PacketDotNet.Packet.ParsePacket(rawPacket.LinkLayerType, rawPacket.Data);

            //Console.WriteLine(BitConverter.ToString(rawPacket.Data));

            if (!this.thisQueue.Contains(BitConverter.ToString(rawPacket.Data)))
            {
                //IN
                //TODO: ACL in check


                this.destQueue.Add(BitConverter.ToString(rawPacket.Data));
                
                //increase in stats
                SW.analizePacket(rawPacket, "in", this, dict_to_update_in);

                //TODO: set max camRecords in table
                if (packet is PacketDotNet.EthernetPacket eth)
                {

                    if (eth.SourceHardwareAddress.ToString() != "02004C4F4F50")
                    {
                        Reset(this.timer);
                    }

                    bool isInTable = false;
                    bool portUpdated = false;

                    //Console.WriteLine("eth packet"+eth.SourceHardwareAddress);

                    var index = this.SW.camTable.camRecords.FindIndex(c => c.macAddress.ToString() == eth.SourceHardwareAddress.ToString());

                    if (index != -1)
                    {
                        isInTable = true;
                        Console.WriteLine("MAC in table");
                        if (SW.camTable.camRecords[index].port != this)
                        {
                            Console.WriteLine("MAC on other port update");
                            portUpdated = true;
                            SW.camTable.camRecords[index].port = this;
                            SW.camTable.camRecords[index].timer = this.SW.camTable.keepTime;
                            CamRecord switched = SW.camTable.camRecords[index];
                            SW.camTable.camRecords.Clear();
                            SW.camTable.camRecords.Add(switched);
                        }
                        else
                        {
                            Console.WriteLine("MAC update");
                            SW.camTable.camRecords[index].timer = this.SW.camTable.keepTime;

                        }
                    }

                    if (!isInTable)
                    {
                        //MAC adresa tohto NB
                        if (eth.SourceHardwareAddress.ToString() != "02004C4F4F50")
                        {
                            Console.WriteLine("Added mac: "+ eth.SourceHardwareAddress);
                            this.SW.camTable.camRecords.Add(new CamRecord(this.SW.camTable.keepTime, this, eth.SourceHardwareAddress));
                        }
                    }

                    //DOTO: ACL out check

                    //sending
                    if (packet is PacketDotNet.EthernetPacket ethernet)
                    {
                        index = this.SW.camTable.camRecords.FindIndex(c => c.macAddress.ToString() == ethernet.DestinationHardwareAddress.ToString());
                        Console.WriteLine("index: "+ index);
                        
                        if (index != -1)
                        {
                            
                            //check if its same port if yes than check for posible cable switched
                            if (SW.camTable.camRecords[index].port == this)
                            {
                                if (portUpdated)
                                {
                                    //unknown unicast because cables maybe switched
                                    Console.WriteLine("Sending as unknown unicast");
                                    this.destDevice.SendPacket(rawPacket);
                                }
                            }                            
                            else
                            {
                                //send to finded port
                                Console.WriteLine("Sending as unicast");
                                this.destDevice.SendPacket(rawPacket);
                            }
                        }
                        else
                        {
                            //TODO:add if for multicast
                            if (ethernet.DestinationHardwareAddress.ToString() == "FFFFFFFFFFFF")
                            {
                                //broadcast
                                Console.WriteLine("Sending as broadcast");
                                this.destDevice.SendPacket(rawPacket);

                            }
                            else
                            {
                                //unknown unicast and also multicast
                                Console.WriteLine("Sending as unknown unicast");
                                this.destDevice.SendPacket(rawPacket);
                            }
                        }
                    }
                    else
                    {
                        //uknown packet
                        Console.WriteLine("Sending as other");
                        this.destDevice.SendPacket(rawPacket);
                    }
                }


                //TODO: CDP

            }
            else
            {
                //OUT
                this.thisQueue.Remove(BitConverter.ToString(rawPacket.Data));
                //increase out stats
                SW.analizePacket(rawPacket, "out", this, dict_to_update_out);
            }
        }
    }
}

