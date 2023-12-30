#include "header/mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMessageBox>
#include <QBEInteger>

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
    QFile file("C:/Users/97798/Desktop/QTP1/Expenditure_Traversing_system/main/temp.txt"); // change path for your system
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
    /*QString id = ui->transaction_ID->text();
    int id_empty = id.isEmpty();

    QString date = QDate::currentDate().toString();
    int date_empty = date.isEmpty();

    QString name = ui->name_input->text();
    int name_empty = name.isEmpty();

    QString address = ui->address_input->text();
    int address_empty = address.isEmpty();

    QString phone = ui->phone_input->text();
    int phone_empty = phone.isEmpty();

    QString amount_1 = ui->amount1_input->text();
    int amount_1_empty = amount_1.isEmpty();

    QString amount_2 = ui->amount2_input->text();
    int amount_2_empty = amount_2.isEmpty();

    double amount_3_number = amount_1.toDouble() - amount_2.toDouble();
    QString amount_3 = QString::number(amount_3_number, 'f', 2);
    int amount_3_empty = amount_3.isEmpty();

    QString remarks = ui->remarks_input->text();*/
    bool all{};

    QVariant  reason, id, amount;
    id = ui->transaction_ID->text().toInt();
    date = ui->date_input->text();
    reason = ui->reason_of_transaction->currentText();
    amount = ui->amount_input->text().toInt();
    all = id.isNull()||date.isNull()||reason.isNull()||amount.isNull();
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
            save_query.prepare("INSERT INTO " + tableName + " (`id`, date, reason, amount) VALUES (:id, :date, :reason, :amount)");
            save_query.bindValue(":id", id);
            save_query.bindValue(":date",date);
            save_query.bindValue(":reason", reason);
            save_query.bindValue(":amount", amount);

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
                // QMessageBox::warning(this, "Error encountered", save_query.lastError().text());
                qDebug() << save_query.lastError().text();

            db_conn_close();
        }
    }
    else
    {
        QMessageBox::warning(this, "Empty input field", "Have you missed something? Please check it.");
    }
}
/*
void MainWindow::on_calculate_net_amount_btn_clicked()
{
    // code repetition; a method can be used for void MainWindow::on_save_button_clicked()
    // and void MainWindow::on_pushButton_clicked() to use the following
    QString amount_1 = ui->amount1_input->text();
    QString amount_2 = ui->amount2_input->text();

    double amount3_number = amount_1.toDouble() - amount_2.toDouble(); // made double
    QString amount3 = QString::number(amount3_number, 'f', 2);
    ui->amount3_input->setText(amount3);
}
*/
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
        QString date = QDate::currentDate().toString();
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
*/
        all = id.isEmpty() || date.isEmpty() || amount.isEmpty();
        if (!db_conn_open())
        {
            qDebug() << "Failed to Open The Database";
            return;
        }

        if (!all)
        {
            db_conn_open();

            QString income_or_expense, reason;
            if (ui->select_expense_btn->isChecked())
            {
                income_or_expense = temp_username + "_ex";
                reason = ui->reason_of_transaction->currentText();
            }
            else if (ui->select_income_btn->isChecked())
            {
                income_or_expense = temp_username + "_in";
                reason = ui->reason_of_transaction->currentText();
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

                if (ui->select_income_btn->isChecked())
                    update_qry.prepare("UPDATE " + income_or_expense + " SET `id`= `"+id+"` date = '" + date + "' reason = '" + reason +"' amount = `"+amount+"` WHERE `id` = `" + id+"`");
                else if (ui->select_expense_btn->isChecked())
                    update_qry.prepare("UPDATE " + income_or_expense + " SET `id`=`"+id+"` date = '" + date + "' reason = '" + reason + "' amount = `"+amount+"` WHERE `id` = `" + id+"`");
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
    QFile file("C:/Users/aakas/OneDrive/Desktop/folders/programming/C++,C/Uni_project/Expenditure_traversing_system/main/text.txt");

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
    QSqlQuery query;
    // Open the SQLite database
    db_conn_open();
    if (!main_db.open())
    {
        qDebug() << "Failed to open database";
        return;
    }
    qDebug()<< QSqlDatabase::drivers();
    filePath = QFileDialog::getSaveFileName(this, "Save PDF", "", "*.pdf");
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Cannot open file for reading: " << qPrintable(file.errorString());
    }

    // Create a PDF writer
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageOrientation(QPageLayout::Portrait);

    // Create a painter to draw on the PDF
    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up the report header
    QFont headerFont("Arial", 14, QFont::Bold);
    painter.setFont(headerFont);
    painter.drawText(QRect(4000, 500, 4000, 500), Qt::AlignLeft, "Financial Report");

    // Set up the table headers
    QFont tableHeaderFont("Arial", 12, QFont::Bold);
    painter.setFont(tableHeaderFont);
    painter.drawText(QRect(4000, 1500, 1000, 300), Qt::AlignLeft, "Date");
    painter.drawText(QRect(5000, 1500, 1000, 300), Qt::AlignLeft, "Type");
    painter.drawText(QRect(6000, 1500, 1000, 300), Qt::AlignLeft, "Amount");

    // Set up the table rows
    QFont tableRowFont("Arial", 10);
    painter.setFont(tableRowFont);
    int row = 0;
    while (query.next())
    {
        QString date = query.value("date").toString();
        QString type = query.value("reason").toString();
        double amount = query.value("amount").toDouble();

        painter.drawText(QRect(3000, 1000 + row * 150, 1000, 200), Qt::AlignLeft, date);
        painter.drawText(QRect(4000, 1000 + row * 150, 1000, 200), Qt::AlignLeft, type);
        painter.drawText(QRect(5000, 1000 + row * 150, 1000, 200), Qt::AlignLeft, QString::number(amount));

        row++;
    }
    int total_inc[2],total_exp[6];
    int i =0;
    QDate Date = QDate::currentDate();


    for(QString reason: list_income){

        //if(query.exec("SELECT SUM(amount) FROM "+MainWindow::temp_username+"_in WHERE date < "+date+" AND reason = "+reason)){
           // total_inc[i] = query.value(0).toInt();
   // }
    i++;
    }
    i = 0;
    for(QString reason : list_expense){

    /*if(query.exec("SELECT SUM(amount) FROM "+MainWindow::temp_username+"_ex WHERE date < "+date+" AND reason = "+reason)){
            total_exp[i] = query.value(0).toInt();
    }*/
    i++;
    }
    double cosineSimilarity{};
    double incomeSum = 0.0, expenseSum = 0.0, productSum = 0.0;
    // Fetch transactions from the income table
    QSqlQuery incomeQuery;
    incomeQuery.prepare(QString("SELECT * FROM %1_in").arg(temp_username));
    if (!incomeQuery.exec())
    {
        qDebug() << "Failed to execute query";
        return;
    }
    while (incomeQuery.next())
    {
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
    if (!expenseQuery.exec())
    {
        qDebug() << "Failed to execute query";
        return;
    }
    while (expenseQuery.next())
    {
        Transaction transaction;
        transaction.date = expenseQuery.value("date").toDate();
        transaction.type = "expense";
        transaction.reason = expenseQuery.value("reason").toString();
        transaction.amount = expenseQuery.value("amount").toDouble();
        transactions.append(transaction);
    }

    // Calculate the total for each income and expense reason
    QMap<QString, double> totals;

    double totalIncome = 0.0;
    int numIncomeTransactions = 0;
    for (Transaction transaction : transactions)
    {
        if (transaction.type == "income")
        {
            totalIncome += transaction.amount;
            numIncomeTransactions++;
        }
    }
    double averageIncome = totalIncome / numIncomeTransactions;
    // Calculate totals for income and expense categories
    QStringList reasons = {"family", "personal", "food", "rent", "travel", "study", "entertainment", "other"};
    QMap<QString, double> incomeTotals = calculateCategoryTotals(temp_username, reasons, "in");
    QMap<QString, double> expenseTotals = calculateCategoryTotals(temp_username, reasons, "ex");

    // Calculate cosine similarity between income and expense vectors
    cosineSimilarity = calculateCosineSimilarity(incomeTotals, expenseTotals);

    // Write the cosine similarity to the PDF
    const int TEXT_X = 2000;
    const int TEXT_Y = 5600;
    const int PIE_RADIUS = 1000;
    const int PIE_X = 4000;
    const int PIE_Y_OFFSET = 4800;
    const int ROW_HEIGHT = 4000;
    const int PIE_Y_EXTRA = 4000;
    const int ANGLE_MULTIPLIER = 1600;
    const int RECT_WIDTH = 1000;
    const int RECT_HEIGHT = 1000;
    const int TOTAL_X = 500;
    const int TOTAL_WIDTH = 1000;
    const int ANALYSIS_Y_EXTRA = 4000;
    const int ANALYSIS_WIDTH = 2000;
    const int ANALYSIS_HEIGHT = 2000;
    QFont analysisFont("Arial", 10);
    painter.setFont(analysisFont);
    int analysisY = PIE_Y_OFFSET + totals.size() * ROW_HEIGHT + ANALYSIS_Y_EXTRA;
    painter.drawText(QRect(TEXT_X, analysisY, ANALYSIS_WIDTH, ANALYSIS_HEIGHT), Qt::AlignLeft,
                     "Cosine Similarity between Income and Expense: " + QString::number(cosineSimilarity));


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
    double percentageChange;
    // Calculate the percentage change in income from the previous period
    if(previousPeriodIncome!=0){
        double percentageChange = (incomeSum - previousPeriodIncome) / previousPeriodIncome * 100.0;
    }
    double allTotals = 0.0;
    for (double total : totals.values())
    {
        allTotals += total;
    }
    // Set up the font
    QFont font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);

    // Draw the average income
    QString averageIncomeText = QString("Average income: %1").arg(averageIncome);
    painter.drawText(3000, 1500, averageIncomeText);

    // Draw the median income
    QString medianIncomeText = QString("Median income: %1").arg(medianIncome);
    painter.drawText(3000, 2000, medianIncomeText);

    // Draw the standard deviation of the income
    QString standardDeviationText = QString("Standard deviation of income: %1").arg(standardDeviation);
    painter.drawText(3000, 2500, standardDeviationText);



    // Draw the percentage change in income from the previous period
    QString percentageChangeText = QString("Percentage change in income from the previous period: %1%").arg(percentageChange);
    painter.drawText(TEXT_X, TEXT_Y, percentageChangeText);

    // Draw a pie slice for each category
    double startAngle = 0.0;
    int pieY = PIE_Y_OFFSET + ROW_HEIGHT;  // Adjust position for pie chart
    for (int i = 0; i < incomeTotals.size(); ++i)
    {
        QString category = incomeTotals.keys()[i];
        double total = incomeTotals[category];
        double sliceAngle = total / totalIncome * 360.0;
        painter.drawPie(PIE_X, pieY, PIE_RADIUS * 2, PIE_RADIUS * 2, startAngle * ANGLE_MULTIPLIER, sliceAngle * ANGLE_MULTIPLIER);
        startAngle += sliceAngle;
    }

    pieY += ROW_HEIGHT + PIE_Y_EXTRA;  // Adjust position for expense pie chart
    for (int i = 0; i < expenseTotals.size(); ++i)
    {
        QString category = expenseTotals.keys()[i];
        double total = expenseTotals[category];
        double sliceAngle = total / allTotals * 360.0;
        painter.drawPie(PIE_X, pieY, PIE_RADIUS * 2, PIE_RADIUS * 2, startAngle * ANGLE_MULTIPLIER, sliceAngle * ANGLE_MULTIPLIER);
        startAngle += sliceAngle;
    }



    // Write each title and total to the PDF
    QFont categoryFont("Arial", 10);
    painter.setFont(categoryFont);
    for (int i = 0; i < totals.size(); ++i)
    {
        QString title = totals.keys()[i];
        double total = totals[title];
        int rowY = PIE_Y_OFFSET + i * ROW_HEIGHT;
        painter.drawText(QRect(TEXT_X, rowY, RECT_WIDTH, RECT_HEIGHT), Qt::AlignLeft, title);
        painter.drawText(QRect(TOTAL_X, rowY, TOTAL_WIDTH, RECT_HEIGHT), Qt::AlignLeft, QString::number(total));
    }

    // Write the cosine similarity to the PDF
    painter.setFont(analysisFont);
    analysisY = PIE_Y_OFFSET + totals.size() * ROW_HEIGHT + ANALYSIS_Y_EXTRA;
    painter.drawText(QRect(TEXT_X, analysisY, ANALYSIS_WIDTH, ANALYSIS_HEIGHT), Qt::AlignLeft,
                     "Cosine Similarity between Income and Expense: " + QString::number(cosineSimilarity));
    // End painting and close the PDF
    painter.end();
    db_conn_close();
    file.close();
}

QMap<QString, double> MainWindow::calculateCategoryTotals(const QString& temp_username, const QStringList& reasons, const QString& tablePrefix) {
    QMap<QString, double> totals;

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
