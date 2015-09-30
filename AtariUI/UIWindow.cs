﻿using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.IO.Pipes;

namespace UIWindow
{
    public partial class UIWindow : Form
    {
        const string pipeBaseName = "atariUI";

        NamedPipeServerStream server;
        NamedPipeClientStream client;
        StreamReader reader;
        StreamWriter writer;

        bool ignoreEvents = false;

        ConcurrentQueue<string> messages = new ConcurrentQueue<string>();

        public UIWindow()
        {
            InitializeComponent();
        }

        private void LaunchServer(string pipeName)
        {
            try
            {
                Console.WriteLine("Creating server: " + pipeName);
                server = new NamedPipeServerStream(pipeName, PipeDirection.InOut, 4);
                Console.WriteLine("Waiting for connection");
                server.WaitForConnection();
                reader = new StreamReader(server);

                Task.Factory.StartNew(() =>
                {
                    Console.WriteLine("Begin server read loop");
                    while (true)
                    {
                        try
                        {
                            var line = reader.ReadLine();
                            messages.Enqueue(line);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine("ServerLoop exception: {0}", ex.Message);
                        }
                    }
                });
            }
            catch (Exception ex)
            {
                Console.WriteLine("LaunchServer exception: {0}", ex.Message);
            }
        }

        private void SendMessage(string message)
        {
            if (ignoreEvents) return;
            if (writer != null)
            {
                try
                {
                    writer.Write(message);
                    writer.Flush();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("SendMessage exception: {0}", ex.Message);
                }
            }
        }

        private void ProcessMessage(string message)
        {
            ignoreEvents = true;
            var parts = message.Split(' ');
            if (parts.Length == 2)
            {
                
            }
            ignoreEvents = false;
        }

        private void LaunchClient(string pipeName)
        {
            try
            {
                Console.WriteLine("Creating client: " + pipeName);
                client = new NamedPipeClientStream(pipeName);
                Console.WriteLine("Connecting to client");
                client.Connect();
                writer = new StreamWriter(client);
            }
            catch (Exception ex)
            {
                Console.WriteLine("LaunchClient exception: {0}", ex.Message);
            }
        }

        private void timerProcessMessages_Tick(object sender, EventArgs e)
        {
            string message;
            if (messages.TryDequeue(out message) && message != null)
            {
                ProcessMessage(message);
            }
        }

        private void timerInitialize_Tick(object sender, EventArgs e)
        {
            timerInitialize.Enabled = false;

            LaunchClient(pipeBaseName + "ReadFromUI");
            LaunchServer(pipeBaseName + "WriteToUI");

            this.Text = "Asset Viewer UI";
        }

        private void UIWindow_Load(object sender, EventArgs e)
        {
            this.Left = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Right - this.Width;
            this.Top = 0;
            this.KeyPreview = true;
        }

        private void timerDisconnectCheck_Tick(object sender, EventArgs e)
        {
            if (server != null && reader != null && !server.IsConnected)
            {
                Application.Exit();
            }
        }

        private void UIWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            SendMessage("terminate");
        }

        private void UIWindow_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                SendMessage("terminate");
                Close();
            }
        }

        private void buttonLoadFrame_Click(object sender, EventArgs e)
        {
            SendMessage("loadFrame " + textBoxReplayIndex.Text + " " + textBoxFrameIndex.Text);
        }

        private void buttonTestAnchor_Click(object sender, EventArgs e)
        {
            SendMessage("testAnchor");
        }
    }
}
