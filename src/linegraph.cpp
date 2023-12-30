#include "header/linegraph.h"
#include "ui_linegraph.h"
//#include <algorithm>

LineGraph::LineGraph(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LineGraph)
{
    ui->setupUi(this);

    QString temp_file_path = "C:/Users/aakas/OneDrive/Desktop/folders/programming/C++,C/Uni_project/Expenditure_Traversing_system/main/temp.txt";
    qDebug() << temp_file_path;
    QFile file(temp_file_path);

    if (!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "File not opened";
    } else {
        QTextStream in(&file);
        temp_username = in.readAll(); //temp_username has been declared in mainwindow.h
        file.close();
        file.remove();
    }

    QString months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    for (auto month: months){
        ui->select_month_menu->addItem(month);
    }
    ui->view_by_menu->addItem("Transaction Amounts");
    ui->view_by_menu->addItem("No. of transaction");
    ui->select_all_btn->setVisible(0);

    QPixmap view_pixmap(":/img/img/eye.png");
    QIcon view_icon(view_pixmap);
    ui->view_btn->setIcon(view_icon);
    ui->view_btn->setIconSize(QSize(24, 24));

    // QPixmap help(":/img/img/question.png");
    // QIcon help_icon(help);
    // ui->help_button->setIcon(help_icon);
    // ui->help_button->setIconSize(QSize(32, 32));
}

LineGraph::~LineGraph()
{
    delete ui;
}

void LineGraph::on_view_btn_clicked()
{   QVector<QString> Reason;
    QVector<double> reason_amt[10];

    ui->customPlot->xAxis->setLabel("Days");//set name of the x-axis

    //checking if the user selected buyer or seller
    QString income_or_expense;
    const bool income_{ui->select_income_btn->isChecked()},expense_{ui->select_expense_btn->isChecked()};
    if (income_){
        income_or_expense = MainWindow::temp_username + "_in";

    }
    else if (ui->select_expense_btn->isChecked()){
        income_or_expense = MainWindow::temp_username + "_ex";

    }
    else if (!ui->select_all_btn->isChecked()){
        QMessageBox::critical(this,"Error","please select one");
    }

    // to get number of days according to the selected month
    int month_number = ui->select_month_menu->currentIndex() + 1; //to get month no.
    int last_day;//stores no. of day in a month
    QVector<double> x_day_numbers;//stores 0 to last_day, collection of x-coordinate values
    if (month_number == 1 || month_number == 3 || month_number == 5 || month_number == 7 || month_number == 8 || month_number == 10 || month_number == 12){
        for(int i = 0; i <= 31; i++ ){ //storing 0 to 31 in x_days_numbers
            x_day_numbers.push_back(i);//adding day number one by one, i.e. 0, 1, 2, ..., 31
        }

        last_day = 31;
        ui->customPlot->xAxis->setRange(0, 31);//x-axis contains value from 0 to 31
    }
    else if (month_number == 4 || month_number == 6 || month_number == 9 || month_number == 11){
        for(int i = 0; i <= 30; i++ ){
            x_day_numbers.push_back(i);
        }

        last_day = 30;
        ui->customPlot->xAxis->setRange(0, 30);//30 days
    }
    else if (month_number == 2){
        //February, leap year (29 days in February) not included
        for(int i = 0; i <= 28; i++ ){
            x_day_numbers.push_back(i);
        }

        last_day = 28;
        ui->customPlot->xAxis->setRange(0, 28);
    }
    QVector<double> y_amount_sums;

    double amount_sums_max{};
    double amount_sums_min{};
    QString temp = ui->view_by_menu->currentText();
    if (temp == "Transaction Amounts"){ //to view graph on the basis of Transaction Amounts whose index in comboBox is 0, so !0 = 1
        //-----------------------------To get values from the database---------------------
        db_conn_open();

        QSqlQuery *qry = new QSqlQuery(main_db);

        QSqlQueryModel *model = new QSqlQueryModel();

        //collections of y-coordinate values; all are associated with the y-axis values

        //to get the origin i.e. (x, y) = (0, 0)
        y_amount_sums.push_back(0.0);

            for (int day = 1; day <= last_day; day++) { // Loop through days
                QString month_number_text, day_number_text;
                // Format day and month numbers
                day_number_text = (day < 10) ? "0" + QString::number(day) : QString::number(day);
                month_number_text = (month_number < 10) ? "0" + QString::number(month_number) : QString::number(month_number);

                // Construct the date in YYYY-MM-DD format
                QString date = "'2023-" + month_number_text + "-" + day_number_text + "'";

                // Prepare and execute the SQL query
                qry->prepare("SELECT amount FROM " + income_or_expense + " WHERE date = " + date);
                 double sum{};

                // Check if the query was successful
                if (qry->exec()) {

                    while(qry->next()){
                    // Retrieve the sum and add it to the vector
                        qDebug()<<qry->value(0).toString();
                        sum += qry->value(0).toDouble();}
                }

               y_amount_sums.push_back(sum);
            }
            db_conn_close();


        for (auto a: y_amount_sums){
           // if(a==0){QMessageBox::critical(this,"Error","a is 0");
            qDebug() << a;
        }


        //==========================================To get minimum and maximum values==============================
        amount_sums_max = y_amount_sums[0];
        amount_sums_min = y_amount_sums[0];


        get_min_max_value_in_vector(amount_sums_min, amount_sums_max, y_amount_sums);//to get minimum and maximum values in y_amount_sums

        // amounts for each reason
        if(ui->select_expense_btn->isChecked()){
            db_conn_open();
            Reason = {"food","entertainment","rent","study","travel","others"};
            QVector<QString> Reason1;
            for(QString items: Reason){
                    QSqlQuery query;
                    if(query.exec("SELECT reason, amount FROM "+ MainWindow::temp_username+"_ex")) {
                        for(int i = 0;i<6;i++){
                            double total = 0.0;
                            while(query.next()) {
                                QString reason = query.value(0).toString();
                                double amount = query.value(1).toDouble();

                                // Now you can use the reason and amount variables
                                // For example, you can add them to your vectors:
                                Reason1.push_back(reason);
                                total +=amount;
                            }
                            reason_amt[i].push_back(total);
                        }
                    } else {
                        qDebug() << "Query failed: " << query.lastError();
                    }
            }
        }

        else if(ui->select_income_btn->isChecked()){
        db_conn_open();
            Reason = {"family","personal"};
            QVector<QString> Reason1;
            for(QString items: Reason){
                QSqlQuery query;
                if(query.exec("SELECT reason, amount FROM "+ MainWindow::temp_username+"_in")) {
                    for(int i = 0;i<2;i++){
                        double total = 0.0;
                    while(query.next()) {
                        QString reason = query.value(0).toString();
                        double amount = query.value(1).toDouble();

                        // Now you can use the reason and amount variables
                        // For example, you can add them to your vectors:
                        Reason1.push_back(reason);
                        total +=amount;
                    }
                     reason_amt[i].push_back(total);
                    }
                } else {
                    qDebug() << "Query failed: " << query.lastError();
                }
            }

        }

          db_conn_close();
        //deletes graph objects and their correspending data
        ui->customPlot->clearPlottables();

        // set title of the graph:
        ui->plot_title->setText("Graph of transaction amounts vs days");

        // set name of y-axis
        ui->customPlot->yAxis->setLabel("Transaction amounts");

        //set range of y-coordinate values from y_min_value to y_max_value
        ui->customPlot->yAxis->setRange(amount_sums_min, amount_sums_max);

        //add graphs and graph data


        // Define colors for income and expense graphs
        QColor incomeColors[] = {Qt::blue, Qt::black};
        QColor expenseColors[] = {Qt::red, Qt::magenta, Qt::yellow, Qt::cyan, Qt::darkCyan};

        // Clear existing graphs and data:
        ui->customPlot->clearPlottables();

        // Add the total amount graph:
        ui->customPlot->addGraph();
        QPen totalPen(Qt::green);
        ui->customPlot->graph(0)->setPen(totalPen);
        ui->customPlot->graph(0)->setName("Total amount");

        // Add income graphs if applicable:
        if (income_) {
            int i =0;
                for (QString items:Reason) {
                ui->customPlot->addGraph();
                QPen incomePen;
                incomePen.setColor(incomeColors[i]);
                ui->customPlot->graph(ui->customPlot->graphCount() - 1)->setPen(incomePen);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setName(items);
                ui->customPlot->graph(ui->customPlot->graphCount() - 1)->setData(x_day_numbers, reason_amt[i]);
                i++;
                }
        }

        // Add expense graphs if applicable:
        if (expense_) {
                int i =0;
                for (QString items:Reason) {
                while (reason_amt[i].size() < x_day_numbers.size()) {
                    reason_amt[i].push_back(0);
                }
                ui->customPlot->addGraph();
                QPen expensePen{};
                expensePen.setColor(expenseColors[i]);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setPen(expensePen);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setName(items);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x_day_numbers, reason_amt[i]);
                i++;
                }

        }

        // Set data for the total amount graph if it exists:
        if (ui->customPlot->graphCount() > 0) {
                ui->customPlot->graph(0)->setData(x_day_numbers, y_amount_sums);
        } else {
                qWarning() << "Failed to create total amount graph!"; // Log a warning if graph creation failed
        }

        // Configure graph appearance and display:
        ui->customPlot->yAxis->setLabel("Transaction amounts");
        ui->customPlot->yAxis->setRange(amount_sums_min, amount_sums_max); // Set y-axis range
        ui->customPlot->legend->setVisible(true);
        ui->customPlot->replot();

        //to display legend/index
        ui->customPlot->legend->setVisible(true);

        //to display graph(s)
        ui->customPlot->replot();
        }


    else if (temp == "No. of transaction"){//to view graph on the basis of No. of transaction whose index in comboBox is 1
        db_conn_open();
        QVector<double> income_count={0.0},expense_count={0.0};
        QSqlQuery *qry = new QSqlQuery(main_db);

        QSqlQueryModel *model = new QSqlQueryModel();

        QVector<double> num_of_transactions_list;//it is associated with the y-axis values
        num_of_transactions_list.push_back(0.0);//first element is 0 to get the origin (x, y) = (0, 0)

        //to add no. of transaction per day to num_of_transactions_list
        for (int day = 1; day <= last_day; day++ ){
            QString month_number_text, day_number_text;
            if (day < 10)//if day number is 1, 2, ..., 9, day_number_text would be 01, 02, ..., 09
                day_number_text = "0" + QString::number(day);
            else if (day >= 10 && day <= 31)//else simply 10, 11, ..., 31
                day_number_text = QString::number(day);

            if (month_number < 10)//if month number is 1, 2, ..., 9, month_number_text would be 01, 02, ..., 09
                month_number_text = "0" + QString::number(month_number);
            else if (month_number >= 10 && month_number <= 12)//else simply 10, 11, 12
                month_number_text = QString::number(month_number);

            // to get date in YYYY-MM-DD format; year is not counted, only month and day are counted
            QString date = "'2023-" + month_number_text + "-" + day_number_text + "'";

            // to get no. of transaction per day
            if(ui->select_all_btn->isChecked()){//if All is selected

                //buyer_count = no. of tranaction with buyers, seller_count = no. of transaction with sellers
                income_count[0] = 0.0;
                expense_count[0] = 0.0;

                qry->prepare("SELECT COUNT (`id`) FROM " + MainWindow::temp_username + "_in WHERE date LIKE " + date);//counting no. of row where given date matches
                qry->exec();
                model->setQuery(*qry);
                income_count[0] = model->data(model->index(0,0)).toDouble();//returns value at index(0,0) i.e. first row, first column

                qry->prepare("SELECT COUNT (`id`) FROM " + MainWindow::temp_username + "_ex WHERE date LIKE " + date);//counting no. of row where given date matches
                qry->exec();
                model->setQuery(*qry);
                expense_count[0] = model->data(model->index(0,0)).toDouble();

                num_of_transactions_list.push_back(income_count[0] + expense_count[0]);//number of transaction including both income and expense
            }
            else if (ui->select_income_btn->isChecked() || ui->select_expense_btn->isChecked()){//if income or expense is selected
                qry->prepare("SELECT COUNT (`id`) FROM " + income_or_expense + " WHERE date LIKE " + date); //add date

                qry->exec();
                model->setQuery(*qry);
                double count = model->data(model->index(0,0)).toDouble();
                num_of_transactions_list.push_back(count);//adding no. of rows/transaction to num_of_transactions_list
            }
        }

        db_conn_close();

        //to get maximum no. of transaction in a month
        double y_max_transaction_num = num_of_transactions_list[0];
        get_max_num_of_transaction(y_max_transaction_num, num_of_transactions_list);

        //deletes graph objects and their corresponding data
        ui->customPlot->clearPlottables();

        // set title of graph
        ui->plot_title->setText("Graph of no. of transaction vs days");

        //add graph to customPlot object
        ui->customPlot->addGraph();
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
        ui->customPlot->graph(0)->setName("No. of transaction");
         ui->customPlot->yAxis->setRange(0, y_max_transaction_num);
        ui->customPlot->yAxis->setLabel("No. of transaction");
        ui->customPlot->graph(0)->setData(x_day_numbers, num_of_transactions_list);
        if(ui->select_all_btn->isChecked()){
            ui->customPlot->addGraph();
            ui->customPlot->graph(1)->setPen(QPen(Qt::cyan));
            ui->customPlot->graph(1)->setName("income");
            ui->customPlot->graph(1)->setData(x_day_numbers,income_count);
            ui->customPlot->legend->setVisible(true);
            ui->customPlot->replot();

            ui->customPlot->addGraph();
            ui->customPlot->graph(2)->setPen(QPen(Qt::black));
            ui->customPlot->graph(2)->setName("expense");
            ui->customPlot->graph(2)->setData(x_day_numbers,expense_count);
            ui->customPlot->legend->setVisible(true);
            ui->customPlot->replot();
        }//set name of the graph

        // set name of the y-axis


        //set range of y-coordinate values from 0 to y_max_transaction_num


        //set points to plot graph


        //to display legend/index
        ui->customPlot->legend->setVisible(true);

        //to display graph(s)
        ui->customPlot->replot();
    }

}



void LineGraph::get_min_max_value_in_vector(double &min_value, double &max_value, QVector<double> &amount_sums){
    //returns minimum and maximum values
    for(const double &each_sum: amount_sums)
    {
        if (max_value < each_sum)
            max_value = each_sum;
        if (min_value > each_sum)
            min_value = each_sum;
    }
}

void LineGraph::on_view_by_menu_currentIndexChanged(int index)
{
    if (!index){ //only option for Buyer and Seller is shown, option for All is not shown
        ui->select_all_btn->setVisible(0);
    }
    else if (index){ //option of Buyer, Seller and All is shown
        ui->select_all_btn->setVisible(1);
    }
}

void LineGraph::get_max_num_of_transaction(double &max_transaction_num, QVector<double> &num_of_transactions_list){
   /* //returns maximum no. of transaction in a month
    for (const double &transaction_per_day: num_of_transactions_list){
        if (max_transaction_num < transaction_per_day)
            max_transaction_num = transaction_per_day;
    }*/
}

void LineGraph::on_help_button_clicked()
{
    QString path_to_manual = "file:///" + QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("manual.pdf");
    QDesktopServices::openUrl(QUrl(path_to_manual, QUrl::TolerantMode));
}
