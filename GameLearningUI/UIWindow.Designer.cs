namespace UIWindow
{
    partial class UIWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.timerProcessMessages = new System.Windows.Forms.Timer(this.components);
            this.timerInitialize = new System.Windows.Forms.Timer(this.components);
            this.timerDisconnectCheck = new System.Windows.Forms.Timer(this.components);
            this.buttonLoadSignature = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxSignature = new System.Windows.Forms.TextBox();
            this.checkBoxShowBoxes = new System.Windows.Forms.CheckBox();
            this.checkBoxShowFullMesh = new System.Windows.Forms.CheckBox();
            this.checkBoxSelectionOnly = new System.Windows.Forms.CheckBox();
            this.checkBoxTrackable = new System.Windows.Forms.CheckBox();
            this.checkBoxShowCharacterSegments = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxFrameIndex = new System.Windows.Forms.TextBox();
            this.buttonLoadFrame = new System.Windows.Forms.Button();
            this.checkBoxAnimationLabel = new System.Windows.Forms.CheckBox();
            this.buttonTestAnchor = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxReplayIndex = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // timerProcessMessages
            // 
            this.timerProcessMessages.Enabled = true;
            this.timerProcessMessages.Interval = 1;
            this.timerProcessMessages.Tick += new System.EventHandler(this.timerProcessMessages_Tick);
            // 
            // timerInitialize
            // 
            this.timerInitialize.Enabled = true;
            this.timerInitialize.Tick += new System.EventHandler(this.timerInitialize_Tick);
            // 
            // timerDisconnectCheck
            // 
            this.timerDisconnectCheck.Enabled = true;
            this.timerDisconnectCheck.Interval = 500;
            this.timerDisconnectCheck.Tick += new System.EventHandler(this.timerDisconnectCheck_Tick);
            // 
            // buttonLoadSignature
            // 
            this.buttonLoadSignature.Location = new System.Drawing.Point(314, 12);
            this.buttonLoadSignature.Name = "buttonLoadSignature";
            this.buttonLoadSignature.Size = new System.Drawing.Size(82, 27);
            this.buttonLoadSignature.TabIndex = 0;
            this.buttonLoadSignature.Text = "Load";
            this.buttonLoadSignature.UseVisualStyleBackColor = true;
            this.buttonLoadSignature.Click += new System.EventHandler(this.buttonLoadSignature_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(62, 15);
            this.label1.TabIndex = 1;
            this.label1.Text = "Signature:";
            // 
            // textBoxSignature
            // 
            this.textBoxSignature.Location = new System.Drawing.Point(83, 15);
            this.textBoxSignature.Name = "textBoxSignature";
            this.textBoxSignature.Size = new System.Drawing.Size(219, 23);
            this.textBoxSignature.TabIndex = 2;
            // 
            // checkBoxShowBoxes
            // 
            this.checkBoxShowBoxes.AutoSize = true;
            this.checkBoxShowBoxes.Location = new System.Drawing.Point(12, 106);
            this.checkBoxShowBoxes.Name = "checkBoxShowBoxes";
            this.checkBoxShowBoxes.Size = new System.Drawing.Size(58, 19);
            this.checkBoxShowBoxes.TabIndex = 3;
            this.checkBoxShowBoxes.Text = "Boxes";
            this.checkBoxShowBoxes.UseVisualStyleBackColor = true;
            this.checkBoxShowBoxes.CheckedChanged += new System.EventHandler(this.checkBoxShowBoxes_CheckedChanged);
            // 
            // checkBoxShowFullMesh
            // 
            this.checkBoxShowFullMesh.AutoSize = true;
            this.checkBoxShowFullMesh.Location = new System.Drawing.Point(76, 106);
            this.checkBoxShowFullMesh.Name = "checkBoxShowFullMesh";
            this.checkBoxShowFullMesh.Size = new System.Drawing.Size(79, 19);
            this.checkBoxShowFullMesh.TabIndex = 3;
            this.checkBoxShowFullMesh.Text = "Full mesh";
            this.checkBoxShowFullMesh.UseVisualStyleBackColor = true;
            this.checkBoxShowFullMesh.CheckedChanged += new System.EventHandler(this.checkBoxShowFullMesh_CheckedChanged);
            // 
            // checkBoxSelectionOnly
            // 
            this.checkBoxSelectionOnly.AutoSize = true;
            this.checkBoxSelectionOnly.Location = new System.Drawing.Point(161, 106);
            this.checkBoxSelectionOnly.Name = "checkBoxSelectionOnly";
            this.checkBoxSelectionOnly.Size = new System.Drawing.Size(102, 19);
            this.checkBoxSelectionOnly.TabIndex = 3;
            this.checkBoxSelectionOnly.Text = "Selection only";
            this.checkBoxSelectionOnly.UseVisualStyleBackColor = true;
            this.checkBoxSelectionOnly.CheckedChanged += new System.EventHandler(this.checkBoxSelectionOnly_CheckedChanged);
            // 
            // checkBoxTrackable
            // 
            this.checkBoxTrackable.AutoSize = true;
            this.checkBoxTrackable.Location = new System.Drawing.Point(269, 106);
            this.checkBoxTrackable.Name = "checkBoxTrackable";
            this.checkBoxTrackable.Size = new System.Drawing.Size(79, 19);
            this.checkBoxTrackable.TabIndex = 3;
            this.checkBoxTrackable.Text = "Trackable";
            this.checkBoxTrackable.UseVisualStyleBackColor = true;
            this.checkBoxTrackable.CheckedChanged += new System.EventHandler(this.checkBoxTrackable_CheckedChanged);
            // 
            // checkBoxShowCharacterSegments
            // 
            this.checkBoxShowCharacterSegments.AutoSize = true;
            this.checkBoxShowCharacterSegments.Location = new System.Drawing.Point(12, 131);
            this.checkBoxShowCharacterSegments.Name = "checkBoxShowCharacterSegments";
            this.checkBoxShowCharacterSegments.Size = new System.Drawing.Size(134, 19);
            this.checkBoxShowCharacterSegments.TabIndex = 3;
            this.checkBoxShowCharacterSegments.Text = "Character segments";
            this.checkBoxShowCharacterSegments.UseVisualStyleBackColor = true;
            this.checkBoxShowCharacterSegments.CheckedChanged += new System.EventHandler(this.checkBoxShowCharacterSegments_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(33, 80);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(44, 15);
            this.label2.TabIndex = 1;
            this.label2.Text = "Frame:";
            // 
            // textBoxFrameIndex
            // 
            this.textBoxFrameIndex.Location = new System.Drawing.Point(83, 77);
            this.textBoxFrameIndex.Name = "textBoxFrameIndex";
            this.textBoxFrameIndex.Size = new System.Drawing.Size(219, 23);
            this.textBoxFrameIndex.TabIndex = 2;
            this.textBoxFrameIndex.Text = "0";
            // 
            // buttonLoadFrame
            // 
            this.buttonLoadFrame.Location = new System.Drawing.Point(314, 58);
            this.buttonLoadFrame.Name = "buttonLoadFrame";
            this.buttonLoadFrame.Size = new System.Drawing.Size(82, 27);
            this.buttonLoadFrame.TabIndex = 0;
            this.buttonLoadFrame.Text = "Load";
            this.buttonLoadFrame.UseVisualStyleBackColor = true;
            this.buttonLoadFrame.Click += new System.EventHandler(this.buttonLoadFrame_Click);
            // 
            // checkBoxAnimationLabel
            // 
            this.checkBoxAnimationLabel.AutoSize = true;
            this.checkBoxAnimationLabel.Location = new System.Drawing.Point(152, 131);
            this.checkBoxAnimationLabel.Name = "checkBoxAnimationLabel";
            this.checkBoxAnimationLabel.Size = new System.Drawing.Size(113, 19);
            this.checkBoxAnimationLabel.TabIndex = 3;
            this.checkBoxAnimationLabel.Text = "Animation label";
            this.checkBoxAnimationLabel.UseVisualStyleBackColor = true;
            this.checkBoxAnimationLabel.CheckedChanged += new System.EventHandler(this.checkBoxAnimationLabel_CheckedChanged);
            // 
            // buttonTestAnchor
            // 
            this.buttonTestAnchor.Location = new System.Drawing.Point(12, 156);
            this.buttonTestAnchor.Name = "buttonTestAnchor";
            this.buttonTestAnchor.Size = new System.Drawing.Size(91, 27);
            this.buttonTestAnchor.TabIndex = 0;
            this.buttonTestAnchor.Text = "Test anchor";
            this.buttonTestAnchor.UseVisualStyleBackColor = true;
            this.buttonTestAnchor.Click += new System.EventHandler(this.buttonTestAnchor_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(33, 49);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 15);
            this.label3.TabIndex = 1;
            this.label3.Text = "Replay:";
            // 
            // textBoxReplayIndex
            // 
            this.textBoxReplayIndex.Location = new System.Drawing.Point(83, 46);
            this.textBoxReplayIndex.Name = "textBoxReplayIndex";
            this.textBoxReplayIndex.Size = new System.Drawing.Size(219, 23);
            this.textBoxReplayIndex.TabIndex = 2;
            this.textBoxReplayIndex.Text = "0";
            // 
            // UIWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(408, 382);
            this.Controls.Add(this.checkBoxSelectionOnly);
            this.Controls.Add(this.checkBoxShowCharacterSegments);
            this.Controls.Add(this.checkBoxTrackable);
            this.Controls.Add(this.checkBoxAnimationLabel);
            this.Controls.Add(this.checkBoxShowFullMesh);
            this.Controls.Add(this.checkBoxShowBoxes);
            this.Controls.Add(this.textBoxReplayIndex);
            this.Controls.Add(this.textBoxFrameIndex);
            this.Controls.Add(this.textBoxSignature);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonTestAnchor);
            this.Controls.Add(this.buttonLoadFrame);
            this.Controls.Add(this.buttonLoadSignature);
            this.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "UIWindow";
            this.Opacity = 0.8D;
            this.Text = "Waiting for connection from application...";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UIWindow_FormClosing);
            this.Load += new System.EventHandler(this.UIWindow_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.UIWindow_KeyUp);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timerProcessMessages;
        private System.Windows.Forms.Timer timerInitialize;
        private System.Windows.Forms.Timer timerDisconnectCheck;
        private System.Windows.Forms.Button buttonLoadSignature;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxSignature;
        private System.Windows.Forms.CheckBox checkBoxShowBoxes;
        private System.Windows.Forms.CheckBox checkBoxShowFullMesh;
        private System.Windows.Forms.CheckBox checkBoxSelectionOnly;
        private System.Windows.Forms.CheckBox checkBoxTrackable;
        private System.Windows.Forms.CheckBox checkBoxShowCharacterSegments;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxFrameIndex;
        private System.Windows.Forms.Button buttonLoadFrame;
        private System.Windows.Forms.CheckBox checkBoxAnimationLabel;
        private System.Windows.Forms.Button buttonTestAnchor;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxReplayIndex;
    }
}

