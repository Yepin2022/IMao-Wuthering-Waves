using IMao_WinUI.ViewModels;

using Microsoft.UI.Xaml.Controls;
using System.Diagnostics;

namespace IMao_WinUI.Views;

public sealed partial class FunctionPage : Page
{
    public FunctionViewModel ViewModel
    {
        get;
    }

    public FunctionPage()
    {
        ViewModel = App.GetService<FunctionViewModel>();
        InitializeComponent();
    }

    private void UpdateMinMapItemDataCycle_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
    {
        IMaoCoreAPI.SetMinMapDataUpdateCycle((int)e.NewValue);
    }

    private void UpdateMapItemDataCycle_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
    {
        IMaoCoreAPI.SetMapDataUpdateCycle((int)e.NewValue);
    }

    private void ToggleSwitch_MapShowItem(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        ToggleSwitch toggleSwitch = sender as ToggleSwitch;
        if (toggleSwitch != null)
        {
            if (toggleSwitch.IsOn == true)
            { 
                IMaoCoreAPI.EnabledMapShowItem(true);
            }
            else
            {
                IMaoCoreAPI.EnabledMapShowItem(false);
            }
        }
    }

    private void ToggleSwitch_MinMapShowItem(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        ToggleSwitch toggleSwitch = sender as ToggleSwitch;
        if (toggleSwitch != null)
        {
            if (toggleSwitch.IsOn == true)
            {
                IMaoCoreAPI.EnabledMinMapShowItem(true);
            }
            else
            {
                IMaoCoreAPI.EnabledMinMapShowItem(false);
            }
        }
    }

    private void ToggleSwitch_SetVisibleSavedPoints(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        ToggleSwitch toggleSwitch = sender as ToggleSwitch;
        if (toggleSwitch != null)
        {
            if (toggleSwitch.IsOn == true)
            {
                IMaoCoreAPI.SetVisibleSavedPoints(true);
            }
            else
            {
                IMaoCoreAPI.SetVisibleSavedPoints(false);
            }
        }
    }

    private void Button_SavedRouteJsonName_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        String content = TextBox_SavedRouteJsonName.Text.ToString();
        if (content != null && content != "") {
            IMaoCoreAPI.SetSavedJsonRouteName(content);
        }
    }

    private void Button_OpenRoutesFolder_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        try
        {
            string appDirectory = AppDomain.CurrentDomain.BaseDirectory;

            string routesPath = Path.Combine(appDirectory, "SavedRoutes");

            if (Directory.Exists(routesPath))
            {
                Process.Start(new ProcessStartInfo(routesPath)
                {
                    UseShellExecute = true,  
                    Verb = "open"         
                });
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("Button_OpenRoutesFolder_Click:" + ex.Message);
        }
    }

    private void Button_LoadRoutesData_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        try
        {
            string appDirectory = AppDomain.CurrentDomain.BaseDirectory;

            string routesFolderPath = Path.Combine(appDirectory, "SavedRoutes");

            if (Directory.Exists(routesFolderPath))
            {
                IMaoCoreAPI.LoadJsonRoute();
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("Button_LoadRoutesData_Click:" + ex.Message);
        }

    }
}
