#include "header/mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMessageBox>
#include <QBEInteger>
#include <QPieSlice>
#include<QtCharts/QPieSeries>
#include <QtCharts/QtCharts>
//#include


// #include <QIntValidator>
QString MainWindow::temp_username;

#include "header/login.h"

void createFinancialReport(QString);
void paintPieChart(QPainter *painter, const QRectF &rect, const QList<double> &data);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(), // parent was passed here
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // file stuffs
    QFile file("C:/Users/97798/Desktop/QTP1/Expenditure_Traversing_system/main/text.txt"); // change path for your system
    QString temp_file_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("text.txt");
    qDebug() << temp_file_path;


    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "File not opened";
        qDebug()<<file.errorString();
    }
    else
    {
        QTextStream in(&file);
        temp_username = in.readAll(); // temp_username has been declared in mainwindow.h
        file.close();
        file.remove();
    }

    ui->username_display->setText(temp_username);
    // database connection
    if (!db_conn_open())
        qDebug() << "Database connection failed.";
    else
        qDebug() << "Database connected.";

    QDoubleValidator *number_input_validator = new QDoubleValidator(0.0, 100000000.0, 2, this);
    ui->transaction_ID->setValidator(number_input_validator);
    // ui->amount1_input->setValidator(number_input_validator);
    //  ui->amount2_input->setValidator(number_input_validator);

    // for phone_input, accepts only phone no. as 98xxxxxxxx
    QRegularExpression phone_rx("98\\d{8}");
    QValidator *phone_validator = new QRegularExpressionValidator(phone_rx, this);
    // ui->phone_input->setValidator(phone_validator);

    // for date_input
    QRegularExpression date_rx("[12]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])");
    QValidator *date_validator = new QRegularExpressionValidator(date_rx, this);
    ui->date_input->setValidator(date_validator);

    // ui->name_input->setPlaceholderText("Buyer's name");

    //  ui->amount1_input->setToolTip("Enter the total amount the buyer needs to give you.");
    //  ui->amount2_input->setToolTip("Enter the amount the buyer gave to you.");
    //  ui->amount3_input->setToolTip("Receivable or remaining amount to be received will be displayed here once you click on 'Calculate'.");

    //   ui->amount1_input->setPlaceholderText("Total Amount");
    //   ui->amount2_input->setPlaceholderText("Received");
    // ui->amount3_input->setPlaceholderText("Receivable");

    // ui->amount3_input->setReadOnly(true);

    ui->buyer_or_seller_drop_menu->addItem("income");
    ui->buyer_or_seller_drop_menu->addItem("expense");

    // to hide combo boxes
    ui->sort_by_drop_menu->setVisible(0);
    ui->order_by_drop_menu->setVisible(0);
    ui->filter_by_drop_menu->setVisible(0);
    ui->filter_condn_drop_menu->setVisible(0);

    set_icons();

    // to load data into the table
    on_view_button_clicked();

    list_income << ""
                << "family"
                << "personal";
    list_expense << ""
                 << "rent"
                 << "food"
                 << "entertainment"
                 << "travel"
                 << "study"
                 << "other";

    ui->reason_of_transaction->clear();
    ui->reason_of_transaction->addItems(list_income);
    ui->reason_of_transaction->update();
}

MainWindow::~MainWindow()
{

    delete ui;
    ui= nullptr;
}

void MainWindow::set_icons()
{
    QPixmap transactionIDicon(":/img/img/hashtag.png");
    ui->transaction_ID_icon->setPixmap(transactionIDicon.scaled(24, 24, Qt::KeepAspectRatio));

    QPixmap dateIcon(":/img/img/calendar.png");
    ui->date_icon->setPixmap(dateIcon.scaled(24, 24, Qt::KeepAspectRatio));

    // QPixmap nameIcon(":/img/img/name.png");
    //  ui->name_icon->setPixmap(nameIcon.scaled(24, 24, Qt::KeepAspectRatio));

    //  QPixmap addressIcon(":/img/img/location.png");
    //  ui->address_icon->setPixmap(addressIcon.scaled(24, 24, Qt::KeepAspectRatio));

    //  QPixmap phoneIcon(":/img/img/phone.png");
    // ui->phone_icon->setPixmap(phoneIcon.scaled(24, 24, Qt::KeepAspectRatio));

    QPixmap ruIcon(":/img/img/ru.png");
    // ui->debit_icon->setPixmap(ruIcon.scaled(26, 26, Qt::KeepAspectRatio));
    //  ui->credit_icon->setPixmap(ruIcon.scaled(26, 26, Qt::KeepAspectRatio));
    //  ui->net_amount_icon->setPixmap(ruIcon.scaled(26, 26, Qt::KeepAspectRatio));

    QPixmap remarksIcon(":/img/img/pencil.png");
    ui->remarks_icon->setPixmap(remarksIcon.scaled(24, 24, Qt::KeepAspectRatio));

    QPixmap save_pixmap(":/img/img/save.png");
    QIcon save_icon(save_pixmap);
    ui->save_button->setIcon(save_icon);
    ui->save_button->setIconSize(QSize(24, 24));

    QPixmap update_pixmap(":/img/img/refresh.png");
    QIcon update_icon(update_pixmap);
    ui->update_button->setIcon(update_icon);
    ui->update_button->setIconSize(QSize(24, 24));

    QPixmap delete_pixmap(":/img/img/delete.png");
    QIcon delete_icon(delete_pixmap);
    ui->delete_button->setIcon(delete_icon);
    ui->delete_button->setIconSize(QSize(24, 24));

    QPixmap view_pixmap(":/img/img/eye.png");
    QIcon view_icon(view_pixmap);
    ui->view_button->setIcon(view_icon);
    ui->view_button->setIconSize(QSize(24, 24));

    QPixmap csv_pixmap(":/img/img/csv.png");
    QIcon csv_icon(csv_pixmap);
    //ui->export_csv_btn->setIcon(csv_icon);
    // ui->export_csv_btn->setIconSize(QSize(24, 24));

    QPixmap sort_pixmap(":/img/img/sort.png");
    QIcon sort_icon(sort_pixmap);
    ui->apply_sort_btn->setIcon(sort_icon);
    ui->apply_sort_btn->setIconSize(QSize(24, 24));

    QPixmap filter_pixmap(":/img/img/filter.png");
    QIcon filter_icon(filter_pixmap);
    ui->apply_filter_btn->setIcon(filter_icon);
    ui->apply_filter_btn->setIconSize(QSize(24, 24));

    QPixmap graph_pixmap(":/img/img/line-chart.png");
    QIcon graph_icon(graph_pixmap);
    ui->show_graph_btn->setIcon(graph_icon);
    ui->show_graph_btn->setIconSize(QSize(24, 24));

    QPixmap signout_pixmap(":/img/img/logout.png");
    QIcon signout_icon(signout_pixmap);
    ui->signout_button->setIcon(signout_icon);
    ui->signout_button->setIconSize(QSize(24, 24));

    QPixmap help(":/img/img/question.png");
    QIcon help_icon(help);
    ui->help_button->setIcon(help_icon);
    ui->help_button->setIconSize(QSize(32, 32));
}

void MainWindow::on_save_button_clicked()
{
    QVariant date = QDate::currentDate().toString();
    bool all{};

    QVariant  reason, id, amount,remarks;
    id = ui->transaction_ID->text().toInt();
    date = ui->date_input->text();
    reason = ui->reason_of_transaction->currentText();
    amount = ui->amount_input->text().toInt();
    remarks = ui->remarks_input->text();

    all = id.isNull()||date.isNull()||reason.isNull()||amount.isNull()||remarks.isNull();
    if (!db_conn_open())
    {
        qDebug() << "Database connection failed";
        return;
    }

    if (!all)
    {
        db_conn_open();
        QSqlQuery save_query;
        QString tableName;

        if (ui->select_income_btn->isChecked())
        {
            tableName = temp_username + "_in";
        }
        else if (ui->select_expense_btn->isChecked())
        {
            tableName = temp_username + "_ex";
        }

        if (!tableName.isEmpty())
        {
            save_query.prepare("INSERT INTO " + tableName + " (`id`, date, reason, amount, remarks) VALUES (:id, :date, :reason, :amount,:remarks)");
            save_query.bindValue(":id", id);
            save_query.bindValue(":date",date);
            save_query.bindValue(":reason", reason);
            save_query.bindValue(":amount", amount);
            save_query.bindValue(":remarks", remarks);

            if (save_query.exec())
            {
                qDebug() << "Insertion successful!";
                on_save_button_clicked();
                db_conn_close();
            }
            else
            {
                qDebug() << "Error:" << save_query.lastError().text();
            }
        }
        else
        {
            qDebug() << "No table selected!";
        }

        if (save_query.exec())
        {
            QMessageBox::information(this, " ", "Records added successfully.");
            on_view_button_clicked();
            db_conn_close();
        }
        else
        {
            QString with_income_or_expense = temp_username;
            if (ui->select_income_btn->isChecked())
                with_income_or_expense += "_in";
            else if (ui->select_expense_btn->isChecked())
                with_income_or_expense += "_ex";

            if ("UNIQUE constraint failed: " + with_income_or_expense + ".Transaction ID Unable to fetch row" == save_query.lastError().text())
                QMessageBox::warning(this, "Same Transaction ID", "Oops, you have a record with same Transaction ID. Try a different one.");
            else
                qDebug() << save_query.lastError().text();

            db_conn_close();
        }
    }
    else
    {
        QMessageBox::warning(this, "Empty input field", "Have you missed something? Please check it.");
    }
}
void MainWindow::on_update_button_clicked()
{
    // ui->transaction_ID->setReadOnly(true); //makes line edit non-editable
    // works only for fixed id, we cannot change id; limitation
    // asking user for confirmation
    QMessageBox::StandardButton reply_from_user = QMessageBox::question(this, " ", "Do you want to update the selected record?", QMessageBox::Yes | QMessageBox::No);

    if (reply_from_user == QMessageBox::Yes)
    {
        bool all{};

        QString id = ui->transaction_ID->text();
        QString date = ui->date_input->text();
        // QString name = ui->name_input->text();
        // int name_empty = name.isEmpty();
        QString amount = ui->amount_input->text();
        /*

        QString amount_2 = ui->amount2_input->text();
        int amount_2_empty = amount_2.isEmpty();

        double amount_3_number = amount_1.toDouble() - amount_2.toDouble();
        QString amount_3 = QString::number(amount_3_number, 'f', 2);
        int amount_3_empty = amount_3.isEmpty();

        QString remarks = ui->remarks_input->text();
*/      QString reason = ui->reason_of_transaction->currentText();
        QString remarks = ui->remarks_input->text();
        all = id.isEmpty() || date.isEmpty() || amount.isEmpty()||reason.isEmpty()||remarks.isEmpty();

        if (!db_conn_open())
        {
            qDebug() << "Failed to Open The Database";
            return;
        }

        if (!all)
        {
            db_conn_open();

            QString income_or_expense;
            if (ui->select_expense_btn->isChecked())
            {
                income_or_expense = MainWindow::temp_username + "_ex";

            }
            else if (ui->select_income_btn->isChecked())
            {
                income_or_expense = MainWindow::temp_username + "_in";

            }

            QSqlQuery check_transaction_id_qry;
            check_transaction_id_qry.prepare("SELECT * from " + income_or_expense + " WHERE `id` = " + id);

            int transaction_id_count = 0;
            if (check_transaction_id_qry.exec())
            { // checking if the given Transaction ID is available
                while (check_transaction_id_qry.next())
                {
                    transaction_id_count++;
                }
            }

            if (transaction_id_count)
            { // if the given transaction id is found
                QSqlQuery update_qry;

                if (ui->select_income_btn->isChecked()){
                    update_qry.prepare("UPDATE " + income_or_expense + " SET date = '" + date + "' reason = '" + reason +"' amount = `"+amount+"` remarks = "+remarks+" WHERE `id` = `" + id+"`");}
                else if (ui->select_expense_btn->isChecked())
                {update_qry.prepare("UPDATE " + income_or_expense + " SET  date = '" + date + "' reason = '" + reason + "' amount = `"+amount+"` remarks = "+remarks+" WHERE `id` = `" + id+"`");}
                if (update_qry.exec())
                {
                    QMessageBox::information(this, tr(" "), tr("Record has been updated successfully."));
                    db_conn_close();

                    on_view_button_clicked();
                }
                else
                {
                    QMessageBox::information(this, "Error encountered", update_qry.lastError().text());
                    db_conn_close();
                }
            }
            else
            {
                QMessageBox::warning(this, "Invalid Transaction ID", "Provided Transaction ID could not be found.");
                db_conn_close();
            }
        }
        else
        {
            if (id.isEmpty())
                QMessageBox::warning(this, "No Transaction ID", "Transaction ID is not provided.");
            else
                QMessageBox::warning(this, "Empty input field", "Have you missed something? Please check it.");
        }
    }
    else
    {
        return;
    }
}

void MainWindow::on_delete_button_clicked()
{
    // asking user for confirmation
    QMessageBox::StandardButton reply_from_user = QMessageBox::question(this, " ", "Do you want to deleted the selected record?", QMessageBox::Yes | QMessageBox::No);

    if (reply_from_user == QMessageBox::Yes)
    {
        QString income_expense;
        if (ui->select_expense_btn->isChecked())
            income_expense = temp_username + "_ex";
        else if (ui->select_income_btn->isChecked())
            income_expense = temp_username + "_in";

        QString id = ui->transaction_ID->text();
        int id_empty = id.isEmpty();

        if (!id_empty)
        { // if the user provides Transaction ID
            if (!db_conn_open())
            {
                qDebug() << "Failed to Open The Database";
                return;
            }

            db_conn_open();

            QSqlQuery check_transaction_id_qry;
            check_transaction_id_qry.prepare("SELECT * from " + income_expense + " WHERE `id` = " + id);

            int transaction_id_count = 0;
            if (check_transaction_id_qry.exec())
            { // checking if the given Transaction ID is available
                while (check_transaction_id_qry.next())
                {
                    transaction_id_count++;
                }
            }

            if (transaction_id_count)
            { // if given Transaction ID is found
                QSqlQuery delete_qry;

                if (ui->select_expense_btn->isChecked())
                    delete_qry.prepare("DELETE FROM " + MainWindow::temp_username + "_ex WHERE `id` = " + id);
                else if (ui->select_income_btn->isChecked())
                    delete_qry.prepare("DELETE FROM " + MainWindow::temp_username + "_in WHERE `id` = " + id);

                if (delete_qry.exec())
                {
                    on_view_button_clicked(); // method of View button; void MainWindow::on_view_button_clicked();
                    QMessageBox::information(this, tr(" "), tr("Selected record deleted sucessfully."));
                    db_conn_close();
                }
                else
                {
                    QMessageBox::information(this, tr("Error encountered"), delete_qry.lastError().text());
                    db_conn_close();
                }
            }
            else
            {
                QMessageBox::warning(this, "Invalid Transaction ID", "Provided Transaction ID could not be found.");
                db_conn_close();
            }
        }
        else
        {
            QMessageBox::warning(this, "No Transaction ID", "Transaction ID is not provided.");
        }
    }
    else
    {
        return;
    }
}

void MainWindow::on_view_button_clicked()
{

    QSqlQueryModel *model = new QSqlQueryModel();

    db_conn_open();
    QSqlQuery *qry = new QSqlQuery(main_db);

    int income_or_expense_index = ui->buyer_or_seller_drop_menu->currentIndex();
    QString income_or_expense;

    if (!income_or_expense_index)
    { // for income
        income_or_expense = MainWindow::temp_username + "_in";
    }
    else if (income_or_expense_index)
    { // for expense
        income_or_expense = MainWindow::temp_username + "_ex";
    }

    qry->prepare("SELECT * FROM " + income_or_expense);
    qry->exec();
    model->setQuery(*qry);
    ui->display_area->setModel(model);
    db_conn_close();
}

void MainWindow::on_sort_by_drop_menu_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->order_by_drop_menu->clear();
        ui->order_by_drop_menu->addItem("Lowest first");
        ui->order_by_drop_menu->addItem("Highest first");
    }

    else if (index == 2)
    {
        ui->order_by_drop_menu->clear();
        ui->order_by_drop_menu->addItem("Alphabetical");
        ui->order_by_drop_menu->addItem("Reverse Alphabetical");
    }
    else if (index == 1)
    {
        ui->order_by_drop_menu->clear();
        ui->order_by_drop_menu->addItem("Recent last");
        ui->order_by_drop_menu->addItem("Recent first");
    }
}

void MainWindow::on_filter_by_drop_menu_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->filter_condn_drop_menu->clear();
        QString transaction_ID_ranges[20] = {"1-100", "100-200", "200-300", "300-400", "400-500", "500-600", "600-700", "700-800", "800-900", "900-1000",
                                             "1000-1100", "1100-1200", "1200-1300", "1300-1400", "1400-1500", "1500-1600", "1600-1700", "1700-1800", "1800-1900", "1900-2000"};
        for (auto &transaction_ID_range : transaction_ID_ranges)
        {
            ui->filter_condn_drop_menu->addItem(transaction_ID_range);
        }
    }
    else if (index == 1)
    {
        ui->filter_condn_drop_menu->clear();
        QString months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        for (auto &month : months)
        {
            ui->filter_condn_drop_menu->addItem(month);
        }
    }
    else if (index == 2)
    {
        ui->filter_condn_drop_menu->clear();
        QString alpha_ranges[5] = {"A-E", "F-J", "K-O", "P-T", "U-Z"};
        for (auto &alpha_range : alpha_ranges)
        {
            ui->filter_condn_drop_menu->addItem(alpha_range);
        }
    }
    else if (index == 3)
    {
        ui->filter_condn_drop_menu->clear();
        QString amount_options[8] = {"0-1000", "1000-5000", "5000-10000", "10000-20000", "20000-40000", "40000-80000", "80000-100000", "100000-More"};
    }
    else if (index == 7)
    {
        ui->filter_condn_drop_menu->clear();
        ui->filter_condn_drop_menu->addItem("income");
        ui->filter_condn_drop_menu->addItem("expense");
    }
}

void MainWindow::on_display_area_activated(const QModelIndex &index)
{
    QString val = ui->display_area->model()->data(index).toString();

    if (!index.column())
    { // index.column() returns column number; runs only if the selected column is Trasaction ID whose column no. is 0, so !0 = 1

        if (!db_conn_open())
        {
            qDebug() << "Database connection failed";
            return;
        }

        db_conn_open();

        QSqlQuery qry;

        if (!ui->buyer_or_seller_drop_menu->currentIndex())
        { // for buyer
            qry.prepare("SELECT * FROM " + temp_username + "_ex WHERE `Transaction ID` = '" + val + "'");
            ui->select_expense_btn->setChecked(true);
            ui->reason_of_transaction->addItems(list_income);
        }
        else if (ui->buyer_or_seller_drop_menu->currentIndex())
        { // for seller
            qry.prepare("SELECT * FROM " + temp_username + "_in WHERE `Transaction ID` = '" + val + "'");
            ui->select_income_btn->setChecked(true);
            ui->reason_of_transaction->addItems(list_expense);
        }

        if (qry.exec())
        {
            while (qry.next())
            {
                ui->transaction_ID->setText(qry.value(0).toString());
                ui->date_input->setText(qry.value(1).toString());
                // ui->name_input->setText(qry.value(2).toString());
                // ui->address_input->setText(qry.value(3).toString());
                // ui->phone_input->setText(qry.value(4).toString());
                // ui->amount1_input->setText(qry.value(5).toString());
                // ui->amount2_input->setText(qry.value(6).toString());
                // ui->amount3_input->setText(qry.value(7).toString());
                // ui->remarks_input->setText(qry.value(8).toString());
            }
            db_conn_close();
        }
        else
        {
            QMessageBox::information(this, tr("Error encountered"), qry.lastError().text());
            db_conn_close();
        }
    }
    else
    {
        QMessageBox::information(this, " ", "Please select desired Transaction ID.");
    }
}

void MainWindow::on_no_sort_filter_btn_clicked()
{
    ui->sort_by_drop_menu->setVisible(0);
    ui->order_by_drop_menu->setVisible(0);
    ui->filter_by_drop_menu->setVisible(0);
    ui->filter_condn_drop_menu->setVisible(0);
}

void MainWindow::on_apply_sort_btn_clicked()
{
    ui->sort_by_drop_menu->setVisible(1);
    ui->order_by_drop_menu->setVisible(1);
    ui->filter_by_drop_menu->setVisible(0);
    ui->filter_condn_drop_menu->setVisible(0);

    ui->buyer_or_seller_drop_menu->setCurrentIndex(0);
    ui->sort_by_drop_menu->clear();
    QString sort_by[] = {"id", "date", "reason", "amount"};
    // adding options in sort_by_opt_drop_menu
    for (auto &sort_by_opt : sort_by)
    {
        ui->sort_by_drop_menu->addItem(sort_by_opt); // sort_and_filter_by array defined in mainwindow.h
    }
}

void MainWindow::on_apply_filter_btn_clicked()
{
    ui->sort_by_drop_menu->setVisible(0);
    ui->order_by_drop_menu->setVisible(0);
    ui->filter_by_drop_menu->setVisible(1);
    ui->filter_condn_drop_menu->setVisible(1);

    ui->buyer_or_seller_drop_menu->setCurrentIndex(0);
    ui->filter_by_drop_menu->clear();
    QString filter_by[] = {"id", "date", "reason", "amount"};
    // adding options in filter_by_drop_menu
    for (auto &filter_by_opt : filter_by)
    {
        ui->filter_by_drop_menu->addItem(filter_by_opt);
    }
}

void MainWindow::on_signout_button_clicked()
{
    QMessageBox::StandardButton reply_from_user = QMessageBox::question(this, " ", "Do you want to signout?", QMessageBox::Yes | QMessageBox::No);
    if (reply_from_user == QMessageBox::Yes)
    {
        // hide();
        close();
        Login *show_login_window = new Login(this);
        show_login_window->show();
    }
}

void MainWindow::on_show_graph_btn_clicked()
{
    QString temp_file_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("temp.txt");
    qDebug() << temp_file_path;
    QFile file("C:/Users/97798/Desktop/QTP1/Expenditure_Traversing_system/main/text.txt");

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "File not opened";
        return;
    }
    else
    {
        qDebug() << "File opened";

        QTextStream out(&file);
        out << temp_username;
        file.flush();
        file.close();

        LineGraph *show_line_graph = new LineGraph(this);
        show_line_graph->show();
        show_line_graph->setWindowTitle("ETS");
    }
}

void MainWindow::on_select_income_btn_clicked()
{
    ui->buyer_or_seller_drop_menu->setCurrentIndex(0);

    ui->amount_input->setToolTip("Enter the total amount");

    ui->reason_of_transaction->clear();
    ui->reason_of_transaction->addItems(list_income);
    ui->reason_of_transaction->update();
}

void MainWindow::on_select_expense_btn_clicked()
{
    ui->buyer_or_seller_drop_menu->setCurrentIndex(1);
    ui->reason_of_transaction->clear();
    ui->reason_of_transaction->addItems(list_expense);
    ui->reason_of_transaction->update();
}

void MainWindow::on_help_button_clicked()
{
    // QString path_to_manual = "file:///" + QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("manual.pdf");
    // QDesktopServices::openUrl(QUrl(path_to_manual, QUrl::TolerantMode));
}

void MainWindow::on_buyer_or_seller_drop_menu_currentIndexChanged(int index)
{
    if (ui->apply_sort_btn->isChecked())
    {
        ui->sort_by_drop_menu->clear();
        if (!index)
        {
            QString sort_by[] = {"id", "date", "reason", "amount"};
            // adding options in sort_by_opt_drop_menu
            for (auto &sort_by_opt : sort_by)
                ui->sort_by_drop_menu->addItem(sort_by_opt);
        }
        else if (index)
        {
            QString sort_by[] = {"id", "date", "reason", "amount"};
            // adding options in sort_by_opt_drop_menu
            for (auto &sort_by_opt : sort_by)
                ui->sort_by_drop_menu->addItem(sort_by_opt);
        }
    }
    else if (ui->apply_filter_btn->isChecked())
    {
        ui->filter_by_drop_menu->clear();
        if (!index)
        {
            QString filter_by[] = {"id", "date", "reason", "amount"};
            // adding options in filter_by_drop_menu
            for (auto &filter_by_opt : filter_by)
                ui->filter_by_drop_menu->addItem(filter_by_opt);
        }
        else if (index)
        {
            QString filter_by[] = {"id", "date", "reason", "amount"};
            // adding options in filter_by_drop_menu
            for (auto &filter_by_opt : filter_by)
                ui->filter_by_drop_menu->addItem(filter_by_opt);
        }
    }
}

void MainWindow::on_show_graph_btn_2_clicked()
{
    static int clicker = 0;
    if (0 == clicker)
    {
        int i = 0;
        /* //QString filePath {"C:/Users/aakas/OneDrive/Desktop/folders/programming/C++,C/Uni_project/ETS/export/"+temp_username+"_report_("+i+").ppt"};

         while(QFile::exists(filePath)){
             ++i;
             //filePath = "C:/Users/aakas/OneDrive/Desktop/folders/programming/C++,C/Uni_project/ETS/export/"+temp_username+"_report_("+i+").ppt";
         }*/
        QString filePath{};
        createFinancialReport(filePath);
    }
    else
    {
        if (QMessageBox::question(this, "Confirm", "already clicked once do you wish to continue") == QMessageBox::Yes)
        {
            clicker = 0;
            on_show_graph_btn_2_clicked();
        }
    }
    clicker++;
}

void MainWindow::createFinancialReport( QString &filePath)
{

    QList<Transaction> transactions;

    // Collect data from the database
    collectData(transactions, MainWindow::temp_username);

    // Process the data
    processData(transactions, MainWindow::temp_username);

    // Generate the PDF report
    writeToPDF(transactions, MainWindow::temp_username, filePath);

    // Inform the user that the report is ready
    QMessageBox::information(this, "Report Generated", "Financial report saved to: " + filePath);
}
QMap<QString, double> MainWindow::calculateCategoryTotals(const QString& temp_username, const QStringList& reasons, const QString& tablePrefix) {
    QMap<QString, double> totals;
    db_conn_open();
    for (const QString& reason : reasons) {
        QSqlQuery query;
        query.prepare(QString("SELECT SUM(amount) FROM %1_%2 WHERE reason = :reason").arg(temp_username, tablePrefix));
        query.bindValue(":reason", reason);

        if (!query.exec()) {
            qDebug() << "Failed to execute query";
            // Handle the error as needed
            return QMap<QString, double>();
        }

        if (query.next()) {
            totals[reason] = query.value(0).toDouble();
        }
    }
    db_conn_close();
    return totals;

}

double MainWindow::calculateCosineSimilarity(const QMap<QString, double>& vector1, const QMap<QString, double>& vector2) {
    double dotProduct = calculateDotProduct(vector1, vector2);
    double magnitude1 = calculateMagnitude(vector1);
    double magnitude2 = calculateMagnitude(vector2);

    // Avoid division by zero
    if (magnitude1 == 0 || magnitude2 == 0) {
        return 0.0;
    }

    return dotProduct / (magnitude1 * magnitude2);
}

double MainWindow::calculateSum(const QMap<QString, double>& vector) {
    double sumOfSquares = 0.0;

    for (auto it = vector.constBegin(); it != vector.constEnd(); ++it) {
        sumOfSquares += it.value();
    }

    return sumOfSquares;
}
double MainWindow::calculateMagnitude(const QMap<QString, double>& vector) {
    double sumOfSquares = 0.0;

    for (auto it = vector.constBegin(); it != vector.constEnd(); ++it) {
        sumOfSquares += it.value() * it.value();
    }

    return std::sqrt(sumOfSquares);
}

double MainWindow::calculateDotProduct(const QMap<QString, double>& vector1, const QMap<QString, double>& vector2) {
    double dotProduct = 0.0;

    for (auto it1 = vector1.constBegin(), it2 = vector2.constBegin(); it1 != vector1.constEnd() && it2 != vector2.constEnd(); ++it1, ++it2) {
        dotProduct += it1.value() * it2.value();
    }

    return dotProduct;
}


void MainWindow::writeToPDF(const QList<Transaction>& transactions, const QString& temp_username, QString& filePath){
    filePath = QFileDialog::getSaveFileName(this, "Save PDF", "", "*.pdf");
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Cannot open file for reading: " << qPrintable(file.errorString());
    }

    db_conn_open();
    QSqlQuery query;
    double totalIncome = 0.0;
    int numIncomeTransactions = 0;
    double totalExpense = 0.0;  // Added for expense calculation
    int numExpenseTransactions = 0;  // Added for expense calculation

    for (Transaction transaction : transactions) {
        if (transaction.type == "income") {
            totalIncome += transaction.amount;
            numIncomeTransactions++;
        } else if (transaction.type == "expense") {  // Added for expense calculation
            totalExpense += transaction.amount;
            numExpenseTransactions++;
        } else {
            qDebug() << "Invalid transaction type: " << transaction.type;  // Handle invalid types
        }
    }

    double averageIncome = totalIncome / numIncomeTransactions;
    double averageExpense = totalExpense / numExpenseTransactions;

    // Calculate totals for income and expense categories
    QStringList reasons = {"family", "personal", "food", "rent", "travel", "study", "entertainment", "other"};
    QMap<QString, double> incomeTotals = calculateCategoryTotals(temp_username, reasons, "in");
    QMap<QString, double> expenseTotals = calculateCategoryTotals(temp_username, reasons, "ex");

    // Calculate cosine similarity between income and expense vectors
    double cosineSimilarity = calculateCosineSimilarity(incomeTotals, expenseTotals);

    // Calculate the median income
    QList<double> incomes;
    for (Transaction transaction : transactions)
    {
        if (transaction.type == "income")
        {
            incomes.append(transaction.amount);
        }
    }

    std::sort(incomes.begin(), incomes.end());
    double medianIncome = incomes[incomes.size() / 2];

    // Calculate the standard deviation of the income
    double sumOfSquares = 0.0;
    for (double income : incomes)
    {
        double deviation = income - averageIncome;
        sumOfSquares += deviation * deviation;
    }

    double variance = sumOfSquares / (incomes.size() - 1);
    double standardDeviation = std::sqrt(variance);

    // Fetch the total income from the previous period from the database
    query.prepare(QString("SELECT SUM(amount) FROM %1_in WHERE date < :startDate").arg(temp_username));
    query.bindValue(":startDate", QDate::currentDate()); // startDate is the start date of the current period
    if (!query.exec() || !query.next())
    {
        qDebug() << "Failed to fetch total income from the previous period";
        if(QMessageBox::question(this,"Confirm","Previous period income not found would you like to continue")==QMessageBox::No){
            return;}
    }
    double previousPeriodIncome = query.value(0).toDouble();
    int percentageChang{};

    // Calculate the percentage change in income from the previous period
    if(previousPeriodIncome!=0){
        percentageChang = (totalIncome - previousPeriodIncome) / previousPeriodIncome * 100.0;
    }

    // Create a PDF writer
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageOrientation(QPageLayout::Portrait);

    // Create a painter to draw on the PDF
    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);
    QFont titleFont("Arial", 14, QFont::Bold);
    QFont headerFont("Arial", 12, QFont::Bold);
    QFont tableFont("Arial", 10);
    int leftMargin = 1000;
    // Set up the report header
    painter.setFont(titleFont);
    painter.drawText(QRect(4000, 500, 4000, 500), Qt::AlignLeft, "Financial Report");

    // Set up table headers
    painter.setFont(headerFont);
    painter.drawText(QRect(leftMargin, 1500, 1000, 300), Qt::AlignLeft, "Date");
    painter.drawText(QRect(leftMargin + 2000, 1500, 1000, 300), Qt::AlignLeft, "Type");
    painter.drawText(QRect(leftMargin + 4000, 1500, 1000, 300), Qt::AlignLeft, "Amount");

    // Set up table rows
    QFont tableRowFont("Arial", 10);
    painter.setFont(tableRowFont);
    int row = 0;
    int tableStartY = 2000;  // Start the table lower down the page
    for (Transaction transaction : transactions) {
        painter.drawText(leftMargin, tableStartY + row * 200,  transaction.date.toString());
        painter.drawText(leftMargin + 2000, tableStartY + row * 200,  transaction.type);
        painter.drawText(leftMargin + 4000, tableStartY + row * 200,  QString::number(transaction.amount));
        row++;
    }
    int additionalTextY = tableStartY + row * 200 + 100; // Start below the table with some space
    painter.drawText(1000, additionalTextY, "Average income: ");
    painter.drawText(leftMargin + 2000, additionalTextY, "Rs. "+QString::number(averageIncome));

    additionalTextY += 200; // Move down for the next line
    painter.drawText(1000, additionalTextY, "Average expense: ");
    painter.drawText(leftMargin + 2000, additionalTextY, "Rs. "+QString::number(averageExpense));

    additionalTextY += 200;
    painter.drawText( 1000, additionalTextY, "Standard deviation of income: ");
    painter.drawText(leftMargin + 2500, additionalTextY, QString::number(standardDeviation));

    additionalTextY += 200;
    painter.drawText(1000, additionalTextY, "Percentage change of income: ");
    painter.drawText(leftMargin + 2500, additionalTextY, QString::number(percentageChang)+"%");

    // Other PDF drawing and writing operations can be added here
    int pieChartY = tableStartY + row * 200 + 1000;
    int pieChartHeight = 6000;

    // Scale the image and draw it into the PDF
    // Adding bar graph to pdf


    // Create bar sets for income and expense
    QtCharts::QBarSet *incomeSet = new QtCharts::QBarSet("Income");
    QtCharts::QBarSet *expenseSet = new QtCharts::QBarSet("Expense");

    // Customize the appearance of the bar sets
    incomeSet->setColor(Qt::blue);
    expenseSet->setColor(Qt::red);

    // Add data to the bar sets
    for (const QString &reason : reasons) {
        *incomeSet << incomeTotals[reason];
        *expenseSet << expenseTotals[reason];
    }

    // Create a bar series and add the bar sets
    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
    series->append(incomeSet);
    series->append(expenseSet);

    // Customize the appearance of the bar series
    series->setBarWidth(0.8);

    // Create a chart, add the series, and customize the chart
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Income and Expense");
    chart->setAnimationOptions(QtCharts::QChart::NoAnimation);

    // Customize the appearance of the chart
    chart->setBackgroundBrush(Qt::lightGray);
    chart->setPlotAreaBackgroundBrush(Qt::white);
    chart->setPlotAreaBackgroundVisible(true);

    // Create a chart view
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Show the chartView and ensure it has a valid size
    chartView->resize(1000, 1000);  // Set a specific size for the chartView
    chartView->show();



    // Render the chart into the PDF
    QPixmap pix = chartView->grab();

    int h = painter.window().height()*0.35;
    int w = h * 1.1;
    int x = (painter.window().width() / 2) - (w/2);
    int y = pieChartY;  // Use the y-coordinate for the pie chart
    //chartView->resize(w, h);
    //QThread::msleep(500);
    painter.drawPixmap(x, y, w, h, pix);
    //    chartView->render(&painter, QRect(x, y, w, h));
    QThread::msleep(100);



    // Draw a heading for the cosine similarity section
    if (transactions.size() > 1) {  // Ensure enough transactions for comparison
        QMap<QString, double> vector1, vector2;

        vector1 = calculateCategoryTotals(temp_username, reasons, "in");
        vector2 = calculateCategoryTotals(temp_username, reasons, "ex");
        double cosineSimilarity = calculateCosineSimilarity(vector1, vector2);
        double v1sum = calculateSum(vector1);
        double v2sum = calculateSum(vector2);

        // Draw a heading for the cosine similarity section
        painter.setFont(headerFont);
        painter.drawText(leftMargin, pieChartY + pieChartHeight , "Cosine Similarity:");  // Adjust the vertical position
        // Draw the cosine similarity value below the pie chart

        int cosineSimilarityY = pieChartY + pieChartHeight + 300; // Adjust this to move the cosine similarity down
        painter.setFont(tableRowFont);
        painter.drawText(leftMargin, cosineSimilarityY, QString::number(cosineSimilarity));

        // Interpretation of Cosine Similarity
        QString interpretation;
        if (cosineSimilarity == 1) {
            interpretation = "The cosine similarity of 1 indicates a perfect alignment between your income and expenses."
                             " Your financial patterns are exceptionally well-balanced.";
        } else if (cosineSimilarity > 0.8) {
            interpretation = "With a cosine similarity exceeding 0.8,"
                             " your spending closely mirrors your income, suggesting a well-maintained financial equilibrium.";
        } else if (cosineSimilarity > 0.5) {
            interpretation = "A cosine similarity above 0.5"
                             " indicates a reasonable alignment between your income and expenses. Your financial habits are generally balanced.";
        } else if (cosineSimilarity > 0.2) {
            interpretation = "While the cosine similarity is above 0.2, "
                             "there are noticeable variations between your income and spending patterns. Consider fine-tuning your budget for better alignment.";
        } else {
            qDebug() << v1sum << "::" << v2sum;
            if(v1sum<v2sum){
                interpretation = "The relatively low cosine similarity suggests a significant mismatch between your income and spending."
                                 " It's crucial to reassess your financial strategy for better stability.";
            }
            else if (v1sum>v2sum){
                interpretation = "The relatively low cosine similarity suggests a significant mismatch between your income and spending."
                                 "Your financial strategy provides stability to your budget";
            }
        }
        interpretation += "This insight can guide adjustments in budgeting and financial planning. ";
        interpretation += "Consider reassessing your expenses and identifying areas for potential savings. ";
        interpretation += "Ensure that your financial goals align with your current spending habits. ";
        interpretation += "Regularly monitoring and adjusting your budget can help maintain a healthier financial profile. ";
        interpretation += "Seeking professional financial advice may offer valuable insights and strategies for improvement. ";
        interpretation += "Remember, financial well-being is an ongoing process that benefits from proactive management. ";
        interpretation += "Take steps to align your spending with your financial goals for a more secure future. ";
        interpretation += "Understanding these patterns can empower you to make informed decisions for financial stability. ";
        interpretation += "Consider creating a detailed budget to optimize your spending and savings. ";


        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WordWrap);

        // Define the rectangle for drawing text, adjust the width as needed
        int interpretationWidth = 8000; // Adjust the width based on your layout
        QRect interpretationRect(leftMargin, cosineSimilarityY + 200, interpretationWidth, 3000);

        // Draw the interpretation text with word wrapping
        painter.setFont(tableRowFont);
        painter.drawText(interpretationRect, interpretation, textOption);
    }
    QApplication::processEvents();  // Process events to ensure chartView is shown
    // End painting and close the PDF
    painter.end();
    file.close();
    db_conn_close();
    //expenseChartView->close();
    //incomeChartView->close();
}

void MainWindow::processData(const QList<Transaction>& transactions, const QString& temp_username){
    // Calculate totals for income and expense categories
    db_conn_open();
    QSqlQuery query;
    double totalIncome = 0.0;
    int numIncomeTransactions = 0;
    double totalExpense = 0.0;  // Added for expense calculation
    int numExpenseTransactions = 0;  // Added for expense calculation

    for (Transaction transaction : transactions) {
        if (transaction.type == "income") {
            totalIncome += transaction.amount;
            numIncomeTransactions++;
        } else if (transaction.type == "expense") {  // Added for expense calculation
            totalExpense += transaction.amount;
            numExpenseTransactions++;
        } else {
            qDebug() << "Invalid transaction type: " << transaction.type;  // Handle invalid types
        }
    }

    double averageIncome = totalIncome / numIncomeTransactions;
    double averageExpense = totalExpense / numExpenseTransactions;

    // Calculate totals for income and expense categories
    QStringList reasons = {"family", "personal", "food", "rent", "travel", "study", "entertainment", "other"};
    QMap<QString, double> incomeTotals = calculateCategoryTotals(temp_username, reasons, "in");
    QMap<QString, double> expenseTotals = calculateCategoryTotals(temp_username, reasons, "ex");

    // Calculate cosine similarity between income and expense vectors
    double cosineSimilarity = calculateCosineSimilarity(incomeTotals, expenseTotals);

    // Calculate the median income
    QList<double> incomes;
    for (Transaction transaction : transactions)
    {
        if (transaction.type == "income")
        {
            incomes.append(transaction.amount);
        }
    }

    std::sort(incomes.begin(), incomes.end());
    double medianIncome = incomes[incomes.size() / 2];

    // Calculate the standard deviation of the income
    double sumOfSquares = 0.0;
    for (double income : incomes)
    {
        double deviation = income - averageIncome;
        sumOfSquares += deviation * deviation;
    }

    double variance = sumOfSquares / (incomes.size() - 1);
    double standardDeviation = std::sqrt(variance);

    // Fetch the total income from the previous period from the database
    query.prepare(QString("SELECT SUM(amount) FROM %1_in WHERE date < :startDate").arg(temp_username));
    query.bindValue(":startDate", QDate::currentDate()); // startDate is the start date of the current period
    double percentageChange;
    db_conn_close();

}

void MainWindow::collectData(QList<Transaction>& transactions, const QString& temp_username) {
    QSqlQuery query;

    // Open the SQLite database
    db_conn_open();
    if (!main_db.open()) {
        qDebug() << "Failed to open database";
        return;
    }

    // Fetch transactions from the income table
    QSqlQuery incomeQuery;
    incomeQuery.prepare(QString("SELECT * FROM %1_in").arg(temp_username));
    if (!incomeQuery.exec()) {
        qDebug() << "Failed to execute income query: " << incomeQuery.lastError().text(); // Add error details
        return;
    }
    while (incomeQuery.next()) {
        Transaction transaction;
        transaction.date = incomeQuery.value("date").toDate();
        transaction.type = "income";
        transaction.reason = incomeQuery.value("reason").toString();
        transaction.amount = incomeQuery.value("amount").toDouble();
        transactions.append(transaction);
    }

    // Fetch transactions from the expense table
    QSqlQuery expenseQuery;
    expenseQuery.prepare(QString("SELECT * FROM %1_ex").arg(temp_username));
    if (!expenseQuery.exec()) {
        qDebug() << "Failed to execute expense query: " << expenseQuery.lastError().text(); // Add error details
        return;
    }
    while (expenseQuery.next()) {
        Transaction transaction;
        transaction.date = expenseQuery.value("date").toDate();
        transaction.type = "expense";
        transaction.reason = expenseQuery.value("reason").toString();
        transaction.amount = expenseQuery.value("amount").toDouble();
        transactions.append(transaction);
    }
    // Close the database connection
    db_conn_close();
}
