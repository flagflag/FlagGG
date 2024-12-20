﻿using FlagGG;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI.Xaml.Shapes;

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;

using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using WinUIEx;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace FlagGG;
/// <summary>
/// Provides application-specific behavior to supplement the default Application class.
/// </summary>
public partial class App : Application
{
    private delegate void SetupFinish(nint windowHandle);

    [LibraryImport("Game.dll")]
    private static partial int CSharpEntryPoint(SetupFinish setupFinish);

    [LibraryImport("Game.dll")]
    private static partial void AttachTo(nint windowHandle, nint targetWindowHandle, int x, int y, int width, int height);

    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    public App()
    {
        this.InitializeComponent();
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="args">Details about the launch request and process.</param>
    protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
    {
        mainWindow_ = new MainWindow();
        mainWindow_.Activate();

#if true
        StartGameThread();
#endif
    }

    private void StartGameThread()
    {
        Thread thread = new Thread(() =>
        {
            CSharpEntryPoint((nint gameWindowHandle) =>
            {
                AttachTo(mainWindow_.GetWindowHandle(), gameWindowHandle, 0, 0, 400, 1200);
            });
            Environment.Exit(0);
        });
        thread.Start();
    }

    private Window mainWindow_;
}
