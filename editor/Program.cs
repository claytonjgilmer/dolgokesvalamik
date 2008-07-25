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
//            Application.Run(form);
            form.Show();
            libwrapper.game.init(form.get_renderwindow());


            while (form.Created)
            {
                libwrapper.game.update();
                Application.DoEvents();
            }

            libwrapper.game.exit();
        }
    }
}