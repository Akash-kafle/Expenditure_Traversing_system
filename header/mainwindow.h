#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
//#include <QFile>
#include <QFileInfo>
#include "linegraph.h"
#include "ui_linegraph.h"
#include "QFileInfo"
#include "QStringList"

struct Transaction {
    QDate date;
    QString type;
    QString reason; // Add this line
    double amount;

};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
 static QString temp_username;
    QSqlDatabase main_db;
    QStringList list_income{},list_expense{};

    void db_conn_close(){
        main_db.close();
        main_db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    bool db_conn_open(){
        main_db = QSqlDatabase::addDatabase("QSQLITE");
        main_db.setDatabaseName("C:/Users/97798/Desktop/QTP1/Expenditure_Traversing_system/main/main.db");//change path for your system

        QString db_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("main.db");
        qDebug() << db_path;
        //main_db.setDatabaseName(db_path);

        if (!main_db.open()){
            qDebug() << "Database not connected";
            return false;
        }
        else {
            qDebug() << "Database connected";
            return true;
        }
    }

    void set_icons();

//  Don't know what to pass
//  void view(parameters);
//  void export_as(parameters);

    void view_export(int ve); //ve = view, export; ve gets either 0 or 1

    //void sort_view_export(int sve); //sve = sort, view, export; svs gets either 0 or 1

    //void filter_view_export(int fve); //fve = filter, view, export; fvs gets either 0 or 1

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_save_button_clicked();

    void on_update_button_clicked();

    void on_delete_button_clicked();

    void on_view_button_clicked();

    //void on_export_csv_btn_clicked();

    //void on_calculate_net_amount_btn_clicked();

    void on_sort_by_drop_menu_currentIndexChanged(int index);

    void on_filter_by_drop_menu_currentIndexChanged(int index);

    void on_display_area_activated(const QModelIndex &index);

    void on_no_sort_filter_btn_clicked();

    void on_apply_sort_btn_clicked();

    void on_apply_filter_btn_clicked();

    void on_signout_button_clicked();

    void on_show_graph_btn_clicked();

    void on_select_income_btn_clicked();

    void on_select_expense_btn_clicked();

    void on_help_button_clicked();

    void on_buyer_or_seller_drop_menu_currentIndexChanged(int index);
    void on_show_graph_btn_2_clicked();
    void createFinancialReport( QString& filePath);
    QMap<QString, double> calculateCategoryTotals(const QString& temp_username, const QStringList& reasons, const QString& tablePrefix);
    double calculateCosineSimilarity(const QMap<QString, double>& vector1, const QMap<QString, double>& vector2);
    double calculateMagnitude(const QMap<QString, double>& vector);
    double calculateDotProduct(const QMap<QString, double>& vector1, const QMap<QString, double>& vector2);
    void processData(const QList<Transaction>& transactions, const QString& temp_username);
    void collectData(QList<Transaction>& transactions, const QString& temp_username);
    void writeToPDF(const QList<Transaction>& transactions, const QString& temp_username, QString& filePath);


private:

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
