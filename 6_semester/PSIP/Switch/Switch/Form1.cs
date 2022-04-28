using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Switch
{
    public partial class Form1 : Form
    {
        private Switch SW;
        private bool choosen_interface_1 = false;
        private bool choosen_interface_2 = false;
        
        //private BindingSource cam_table_binding_source = new BindingSource();
        public Form1()
        {
            InitializeComponent();
            this.SW = new Switch(this);
            //this.dGW_CAM_table.DataSource = cam_table_binding_source;


        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        public void interfaceBox_1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //if is not picked 
            //set interface 1 to choosen loopback

            var selected = interfaceBox_1.Text;

            //if switching to same device as int_2 - reset int_2
            if (SW.getDeviceFromList(int.Parse(selected.Split(' ')[2])) == SW.port_2.thisDevice)
            {
                SW.port_2.thisDevice = null;
            }

            //set device to int_1
            SW.port_1.thisDevice = SW.getDeviceFromList(int.Parse(selected.Split(' ')[2]));

            this.choosen_interface_1 = true;
            //Console.WriteLine(int.Parse(selected.Split(' ')[2]));
            //lbl_interfaceBox_1.Text = SW.getDeviceFromList(int.Parse(selected.Split(' ')[2])).Name;  
        }

        private void interfaceBox_2_SelectedIndexChanged(object sender, EventArgs e)
        {
            //if is not picked
            //set interface 2 to choosen loopback

            var selected = interfaceBox_2.Text;

            //if switching to same device as int_1 - reset int_1
            if (SW.getDeviceFromList(int.Parse(selected.Split(' ')[2])) == SW.port_1.thisDevice)
            {
                SW.port_1.thisDevice = null;
            }

            //set device to int_2
            SW.port_2.thisDevice = SW.getDeviceFromList(int.Parse(selected.Split(' ')[2]));
            this.choosen_interface_2 = true;

            //Console.WriteLine(int.Parse(selected.Split(' ')[2]));
        }

        public void update_interfaces(String interface_name)
        {
            interfaceBox_1.Items.Add(interface_name);
            interfaceBox_2.Items.Add(interface_name);

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            SW.get_interfaces();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (this.choosen_interface_1 && this.choosen_interface_2)
            {
                //turn off all not needed buttons,...
                btnStart.Enabled = false;
                btnResetInt1.Enabled = false;
                btnResetInt2.Enabled = false;
                interfaceBox_1.Enabled = false;
                interfaceBox_2.Enabled = false;

                SW.start_listening();
                this.SW.camTable.timer.Start();

                //set red color to turned off buttons,...
                btnStart.BackColor = Color.Red;
                btnResetInt1.BackColor =  Color.Red;
                btnResetInt2.BackColor =  Color.Red;
                interfaceBox_1.BackColor =  Color.Red;
                interfaceBox_2.BackColor =  Color.Red;
            }
        }

        private void label15_Click(object sender, EventArgs e)
        {

        }

        delegate void update_stats_inC_1Callback(Dictionary<string, int> text);
        delegate void update_stats_inC_2Callback(Dictionary<string, int> text);
        delegate void update_stats_outC_1Callback(Dictionary<string, int> text);
        delegate void update_stats_outC_2Callback(Dictionary<string, int> text);
        delegate void update_dGW_CAM_tableCallback(List<CamRecord> text);


        public void update_stats_inC_1(Dictionary<string, int> inC_1)
        {
            if (this.textB_inTotalP_1.InvokeRequired)
            {
                update_stats_inC_1Callback d = new update_stats_inC_1Callback(update_stats_inC_1);
                this.Invoke(d, new object[] { inC_1 });
            }
            else
            {
                //this.textB_inTotalP_1.Text = inC_1;
                foreach (var item in inC_1)
                {
                    switch (item.Key)
                    {
                        case "ETH":
                            this.textB_inEth2_1.Text = (item.Value).ToString();
                            break;
                        case "ARP":
                            this.textB_inARP_1.Text = (item.Value).ToString();
                            break;
                        case "IP":
                            this.textB_inIP_1.Text = (item.Value).ToString();
                            break;
                        case "TCP":
                            this.textB_inTCP_1.Text = (item.Value).ToString();
                            break;
                        case "UDP":
                            this.textB_inUDP_1.Text = (item.Value).ToString();
                            break;
                        case "ICMP":
                            this.textB_inICMP_1.Text = (item.Value).ToString();
                            break;
                        case "HTTP":
                            this.textB_inHTTP_1.Text = (item.Value).ToString();
                            break;
                        case "HTTPS":
                            this.textB_inHTTPS_1.Text = (item.Value).ToString();
                            break;
                        case "TOTAL":
                            this.textB_inTotalP_1.Text = (item.Value).ToString();
                            break;
                        default:
                            break;
                    }
                }
            }
        }


        public void update_stats_inC_2(Dictionary<string, int> inC_2)
        {
            if (this.textB_inTotalP_2.InvokeRequired)
            {
                update_stats_inC_2Callback d = new update_stats_inC_2Callback(update_stats_inC_2);
                this.Invoke(d, new object[] { inC_2 });
            }
            else
            {
                //this.textB_inTotalP_2.Text = inC_2;
                foreach (var item in inC_2)
                {
                    switch (item.Key)
                    {
                        case "ETH":
                            this.textB_inEth2_2.Text = (item.Value).ToString();
                            break;
                        case "ARP":
                            this.textB_inARP_2.Text = (item.Value).ToString();
                            break;
                        case "IP":
                            this.textB_inIP_2.Text = (item.Value).ToString();
                            break;
                        case "TCP":
                            this.textB_inTCP_2.Text = (item.Value).ToString();
                            break;
                        case "UDP":
                            this.textB_inUDP_2.Text = (item.Value).ToString();
                            break;
                        case "ICMP":
                            this.textB_inICMP_2.Text = (item.Value).ToString();
                            break;
                        case "HTTP":
                            this.textB_inHTTP_2.Text = (item.Value).ToString();
                            break;
                        case "HTTPS":
                            this.textB_inHTTPS_2.Text = (item.Value).ToString();
                            break;
                        case "TOTAL":
                            this.textB_inTotalP_2.Text = (item.Value).ToString();
                            break;
                        default:
                            break;
                    }
                }
            }
        }


        public void update_stats_outC_1(Dictionary<string, int> outC_1)
        {
            if (this.textB_outTotalP_1.InvokeRequired)
            {
                update_stats_outC_1Callback d = new update_stats_outC_1Callback(update_stats_outC_1);
                this.Invoke(d, new object[] { outC_1 });
            }
            else
            {
                //this.textB_outTotalP_1.Text = outC_1;
                foreach (var item in outC_1)
                {
                    switch (item.Key)
                    {
                        case "ETH":
                            this.textB_outEth2_1.Text = (item.Value).ToString();
                            break;
                        case "ARP":
                            this.textB_outARP_1.Text = (item.Value).ToString();
                            break;
                        case "IP":
                            this.textB_outIP_1.Text = (item.Value).ToString();
                            break;
                        case "TCP":
                            this.textB_outTCP_1.Text = (item.Value).ToString();
                            break;
                        case "UDP":
                            this.textB_outUDP_1.Text = (item.Value).ToString();
                            break;
                        case "ICMP":
                            this.textB_outICMP_1.Text = (item.Value).ToString();
                            break;
                        case "HTTP":
                            this.textB_outHTTP_1.Text = (item.Value).ToString();
                            break;
                        case "HTTPS":
                            this.textB_outHTTPS_1.Text = (item.Value).ToString();
                            break;
                        case "TOTAL":
                            this.textB_outTotalP_1.Text = (item.Value).ToString();
                            break;
                        default:
                            break;
                    }
                }
            }
        }


        public void update_stats_outC_2(Dictionary<string, int> outC_2)
        {
            if (this.textB_outTotalP_2.InvokeRequired)
            {
                update_stats_outC_2Callback d = new update_stats_outC_2Callback(update_stats_outC_2);
                this.Invoke(d, new object[] { outC_2 });
            }
            else
            {
                // this.textB_outTotalP_2.Text = outC_2;
                foreach (var item in outC_2)
                {
                    switch (item.Key)
                    {
                        case "ETH":
                            this.textB_outEth2_2.Text = (item.Value).ToString();
                            break;
                        case "ARP":
                            this.textB_outARP_2.Text = (item.Value).ToString();
                            break;
                        case "IP":
                            this.textB_outIP_2.Text = (item.Value).ToString();
                            break;
                        case "TCP":
                            this.textB_outTCP_2.Text = (item.Value).ToString();
                            break;
                        case "UDP":
                            this.textB_outUDP_2.Text = (item.Value).ToString();
                            break;
                        case "ICMP":
                            this.textB_outICMP_2.Text = (item.Value).ToString();
                            break;
                        case "HTTP":
                            this.textB_outHTTP_2.Text = (item.Value).ToString();
                            break;
                        case "HTTPS":
                            this.textB_outHTTPS_2.Text = (item.Value).ToString();
                            break;
                        case "TOTAL":
                            this.textB_outTotalP_2.Text = (item.Value).ToString();
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        public void update_dGW_CAM_table(List<CamRecord> camRecords)
        {

            if (this.dGW_CAM_table.InvokeRequired)
            {
                update_dGW_CAM_tableCallback d = new update_dGW_CAM_tableCallback(update_dGW_CAM_table);
                this.Invoke(d, new object[] { camRecords });
            }
            else
            {
                //Console.WriteLine("totok by malo ist");
                var rows = dGW_CAM_table.Rows;

                
                //this.dGW_CAM_table.DataSource = camRecords;
                //this.cam_table_binding_source.DataSource = camRecords;
                foreach (DataGridViewRow row in rows)
                {

                    // Console.WriteLine("This shit "+ row.Cells[1].ToString());
                }
                //this.dGW_CAM_table.Rows.Add();
                dGW_CAM_table.Rows.Clear();
                //dGW_CAM_table
                
                //var index = rows(c => c.Cells[1].ToString() == eth.SourceHardwareAddress.ToString());
                //Console.WriteLine("index: "+ index);


                foreach (var camRecord in camRecords)
                {
                    //this.dGW_CAM_table.D
                    //this.dGW_CAM_table.Rows.Add(camRecord);
                    if (camRecord.port == SW.port_1)
                    {
                        dGW_CAM_table.Rows.Add("1", camRecord.macAddress, camRecord.timer);

                    }
                    else
                    {
                        dGW_CAM_table.Rows.Add("2", camRecord.macAddress, camRecord.timer);
                    }

                }
                //this.dGW_CAM_table.Columns["port"].Visible = false;
            }
                
        }

        private void textBox17_TextChanged(object sender, EventArgs e)
        {

        }

        private void textB_inTotalP_1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textB_inEth2_1_TextChanged(object sender, EventArgs e)
        {

        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            if (this.choosen_interface_1 && this.choosen_interface_2)
            {
                this.SW.stop_listening();

                //SW.port_1.thisDevice = null;
                //SW.port_2.thisDevice = null;

                btnStart.Enabled = true;
                btnResetInt1.Enabled = true;
                btnResetInt2.Enabled = true;
                interfaceBox_1.Enabled = true;
                interfaceBox_2.Enabled = true;

                btnStart.BackColor = Color.Green;
                btnResetInt1.BackColor =  Color.Green;
                btnResetInt2.BackColor =  Color.Green;
                interfaceBox_1.BackColor =  Color.Green;
                interfaceBox_2.BackColor =  Color.Green;

            }

        }

        private void textB_outEth2_1_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void textB_inHTTPS_2_TextChanged(object sender, EventArgs e)
        {

        }

        private void btnResetInt1_Click(object sender, EventArgs e)
        {
            //reset inStatsInt_1
            this.textB_inEth2_1.Text = "0";
            this.textB_inIP_1.Text = "0";
            this.textB_inARP_1.Text = "0";
            this.textB_inTCP_1.Text = "0";
            this.textB_inUDP_1.Text = "0";
            this.textB_inICMP_1.Text = "0";
            this.textB_inHTTP_1.Text = "0";
            this.textB_inHTTPS_1.Text = "0";
            this.textB_inTotalP_1.Text = "0";

            this.SW.port_1.clearStats();

            //reset outStatsInt_1
            this.textB_outEth2_1.Text = "0";
            this.textB_outIP_1.Text = "0";
            this.textB_outARP_1.Text = "0";
            this.textB_outTCP_1.Text = "0";
            this.textB_outUDP_1.Text = "0";
            this.textB_outICMP_1.Text = "0";
            this.textB_outHTTP_1.Text = "0";
            this.textB_outHTTPS_1.Text = "0";
            this.textB_outTotalP_1.Text = "0";
        }

        private void btnResetInt2_Click(object sender, EventArgs e)
        {
            //reset inStatsInt_2
            this.textB_inEth2_2.Text = "0";
            this.textB_inIP_2.Text = "0";
            this.textB_inARP_2.Text = "0";
            this.textB_inTCP_2.Text = "0";
            this.textB_inUDP_2.Text = "0";
            this.textB_inICMP_2.Text = "0";
            this.textB_inHTTP_2.Text = "0";
            this.textB_inHTTPS_2.Text = "0";
            this.textB_inTotalP_2.Text = "0";

            this.SW.port_1.clearStats();

            //reset outStatsInt_2
            this.textB_outEth2_2.Text = "0";
            this.textB_outIP_2.Text = "0";
            this.textB_outARP_2.Text = "0";
            this.textB_outTCP_2.Text = "0";
            this.textB_outUDP_2.Text = "0";
            this.textB_outICMP_2.Text = "0";
            this.textB_outHTTP_2.Text = "0";
            this.textB_outHTTPS_2.Text = "0";
            this.textB_outTotalP_2.Text = "0";
        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void Form1_Leave(object sender, EventArgs e)
        {
            
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.SW.stop_listening();
        }

        private void btnRstMACtbl_Click(object sender, EventArgs e)
        {
            this.SW.camTable.camRecords.Clear();
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            //Console.WriteLine(numericUpDown1.Value);
            this.SW.camTable.keepTime = (int)numericUpDown1.Value;
        }
    }
}
