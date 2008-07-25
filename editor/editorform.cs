using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using libwrapper;

namespace editor
{
    public partial class editorform : Form
    {
        public editorform()
        {
            InitializeComponent();
            property_grid.Initialize();
//            property_grid.SelectedObject = this;
            //            this.Show();
        }

        public IntPtr get_renderwindow()
        {
            return splitContainer1.Panel1.Handle;
        }

        ~editorform()
        {

        }

        void init_engine()
        {

        }

    }
}