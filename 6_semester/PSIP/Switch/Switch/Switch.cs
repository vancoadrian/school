using SharpPcap;
using System;
using System.Collections.Generic;
using System.Threading;
using SharpPcap.LibPcap;

namespace Switch
{
    public class Switch
    {
        public Port port_1;
        public Port port_2;
        private Form1 form;
        public CamTable camTable;

        public Switch(Form1 form)
        {
            this.port_1 = new Port(this);
            this.port_2 = new Port(this);
            this.form = form;
            this.camTable = new CamTable(this);
        }

        /// <summary>
        /// ComboBoxes_load
        /// </summary>
        public void get_interfaces()
        {
            // Retrieve the device list
            var devices = CaptureDeviceList.Instance;

            // If no devices were found print an error
            /*if (devices.Count < 1)
            {
                Console.WriteLine("No devices were found on this machine");
                return;
            }*/

            /*Console.WriteLine();
            Console.WriteLine("The following devices are available on this machine:");
            Console.WriteLine("----------------------------------------------------");
            Console.WriteLine();*/

            int i = 0;

            // Print out the devices
            foreach (var dev in devices)
            {

                /* Description */
                //Console.WriteLine("{0}) {1} {2}", i, dev.Name, dev.Description);
                if (dev.Description.Contains("KM-TEST"))
                {
                    form.update_interfaces("KM-TEST ID: " + i);
                }
                i++;
            }
        }


        public void update_CAM_table(List<CamRecord> camRecords)
        {
            this.form.update_dGW_CAM_table(camRecords);
        }


        public LibPcapLiveDevice getDeviceFromList(int id)
        {
            var devices = LibPcapLiveDeviceList.Instance;
            return devices[id];
        }

        public void start_listening()
        {
            if(this.port_1.thisDevice == null || this.port_2.thisDevice == null)
            {
                //Console.WriteLine("Treba vybrat interfaces");
                return;
            } 

            this.port_1.destDevice = this.port_2.thisDevice;
            this.port_1.destQueue = this.port_2.thisQueue;
            this.port_2.destDevice = this.port_1.thisDevice;
            this.port_2.destQueue = this.port_1.thisQueue;
           
            Console.WriteLine(port_1.thisDevice.Description);
            Console.WriteLine(port_2.thisDevice.Description);

            //Console.WriteLine(port_1.thisDevice.Description);
            //Console.WriteLine(port_2.thisDevice.Description);

            this.port_1.listen();
            this.port_2.listen();
        }


        public void stop_listening()
        {
            if (this.port_1.thisDevice == null || this.port_2.thisDevice == null)
            {
                //Console.WriteLine("Treba vybrat interfaces");
                return;
            }

            //Console.WriteLine("Vypinam threads");

            this.camTable.timer.Stop();
            Thread.Sleep(1);
            this.port_1.stop_listen();
            this.port_2.stop_listen();
        }


        public void analizePacket(RawCapture rawPacket, String direction, Port port_from, Dictionary<string, int> dict_to_update)
        {
            var packet = PacketDotNet.Packet.ParsePacket(rawPacket.LinkLayerType, rawPacket.Data);

            if (packet is PacketDotNet.EthernetPacket eth)
            {
                //Console.WriteLine("ETH");
                dict_to_update["ETH"] += 1;

                //Console.WriteLine("Original Eth packet: " + eth.ToString());
                var arp = packet.Extract<PacketDotNet.ArpPacket>();
                if (arp != null)
                {
                    //Console.WriteLine("ARP");
                    dict_to_update["ARP"] += 1;
                }

                var ip = packet.Extract<PacketDotNet.IPPacket>();
                if (ip != null)
                {
                    //Console.WriteLine("IP");
                    dict_to_update["IP"] += 1;

                    //Console.WriteLine("Original IP packet: " + ip.ToString());

                    if (ip.Protocol == PacketDotNet.ProtocolType.Icmp)
                    {
                        //Console.WriteLine("ICMP");
                        dict_to_update["ICMP"] += 1;
                    }

                    var tcp = packet.Extract<PacketDotNet.TcpPacket>();
                    if (tcp != null)
                    {
                        //Console.WriteLine("Original TCP packet: " + tcp.ToString());
                        //Console.WriteLine("TCP");
                        dict_to_update["TCP"] += 1;

                        if (tcp.SourcePort == 80 || tcp.DestinationPort == 80)
                        {
                            //Console.WriteLine("HTTP");
                            dict_to_update["HTTP"] += 1;
                        }
                        else if(tcp.SourcePort == 443 || tcp.DestinationPort == 443)
                        {
                            //Console.WriteLine("HTTPS");
                            dict_to_update["HTTPS"] += 1;
                        }


                        //manipulate TCP parameters
                        /*
                        tcp.SourcePort;
                        tcp.DestinationPort;
                        tcp.Synchroniz;
                        tcp.Finished;
                        tcp.Acknowledgment;
                        tcp.WindowSize;
                        tcp.AcknowledgmentNumber;
                        tcp.SequenceNumber;
                        */
                    }

                    var udp = packet.Extract<PacketDotNet.UdpPacket>();
                    if (udp != null)
                    {
                        //Console.WriteLine("UDP");
                        dict_to_update["UDP"] += 1;


                        //Console.WriteLine("Original UDP packet: " + udp.ToString());

                        //manipulate UDP parameters
                        //udp.SourcePort;
                        //udp.DestinationPort;
                    }
                }
            }
            dict_to_update["TOTAL"] += 1;


            if (direction=="in")
            {
                if(port_1 == port_from)
                {
                    //inP_1
                    this.form.update_stats_inC_1(dict_to_update);
                }
                else
                {
                    //inP_2
                    this.form.update_stats_inC_2(dict_to_update);
                }
            }
            else
            {
                if (port_1 == port_from)
                {
                    //outP_1
                    this.form.update_stats_outC_1(dict_to_update);
                }
                else
                {
                    //outP_2
                    this.form.update_stats_outC_2(dict_to_update);
                }
            }
        }
    }
}
