namespace editor
{
    partial class editorform
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.property_grid = new editor.editorpropgrid();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(909, 596);
            this.splitContainer1.SplitterDistance = 567;
            this.splitContainer1.TabIndex = 1;
            // 
            // splitContainer2
            // 
            this.splitContainer2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.property_grid);
            this.splitContainer2.Size = new System.Drawing.Size(338, 596);
            this.splitContainer2.SplitterDistance = 190;
            this.splitContainer2.TabIndex = 0;
            // 
            // property_grid
            // 
            this.property_grid.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.property_grid.CommentsBackColor = System.Drawing.SystemColors.Control;
            this.property_grid.CommentsForeColor = System.Drawing.SystemColors.ControlText;
            this.property_grid.CommentsHeight = 40;
            this.property_grid.CommentsVisibility = false;
            this.property_grid.DisplayMode = VisualHint.SmartPropertyGrid.PropertyGrid.DisplayModes.Categorized;
            this.property_grid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.property_grid.EllipsisMode = VisualHint.SmartPropertyGrid.PropertyGrid.EllipsisModes.EllipsisOnValues;
            this.property_grid.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            this.property_grid.GridBackColor = System.Drawing.SystemColors.Window;
            this.property_grid.GridDisabledForeColor = System.Drawing.SystemColors.GrayText;
            this.property_grid.GridForeColor = System.Drawing.SystemColors.ControlText;
            this.property_grid.HighlightedTextColor = System.Drawing.SystemColors.HighlightText;
            this.property_grid.LabelColumnWidth = 150;
            this.property_grid.LeftColumnWidth = 16;
            this.property_grid.Location = new System.Drawing.Point(0, 0);
            this.property_grid.MultiSelectMode = VisualHint.SmartPropertyGrid.PropertyGrid.MultiSelectModes.None;
            this.property_grid.Name = "property_grid";
            this.property_grid.PropertyVerticalMargin = 4;
            this.property_grid.ReadOnlyVisual = VisualHint.SmartPropertyGrid.PropertyGrid.ReadOnlyVisuals.Disabled;
            this.property_grid.SelectedBkgColor = System.Drawing.SystemColors.Highlight;
            this.property_grid.SelectedNotFocusedBkgColor = System.Drawing.SystemColors.ActiveBorder;
            this.property_grid.Size = new System.Drawing.Size(334, 398);
            this.property_grid.TabIndex = 0;
            this.property_grid.ToolbarVisibility = false;
            this.property_grid.ToolTipMode = VisualHint.SmartPropertyGrid.PropertyGrid.ToolTipModes.None;
            // 
            // editorform
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(909, 596);
            this.Controls.Add(this.splitContainer1);
            this.Name = "editorform";
            this.Text = "Form1";
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private editor.editorpropgrid property_grid;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
    }
}

