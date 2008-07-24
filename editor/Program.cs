using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace editor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            editorform form = new editorform();
//            Application.Run(new editorform());
        }
    }
}