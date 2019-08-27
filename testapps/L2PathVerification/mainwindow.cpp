#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ledPWD->setEchoMode(QLineEdit::Password);
    ui->btnDetener->setEnabled(false);
    ui->btnCopy->setEnabled(false);

//    ui->ledPE->setText("10.192.32.133");
//    ui->ledMAC->setText("286e.d47f.cf32");
//    ui->ledUsuario->setText("jideleon");
//    ui->ledPWD->setText("jul+2018");

    lstEquipospos = -1;    
    stop = false;   
    model = nullptr;

    loadingIcon = new QMovie(":/loader.gif");
    loadingIcon->start();
    loadingIcon->stop();
    ui->lblLoading->setMovie(loadingIcon);    

    actualizarInterfaces_timer = new QTimer(this);
    actualizarInterfaces_timer->setInterval( 5000 );
    actualizarInterfaces_timer->setSingleShot(true);
    connect(actualizarInterfaces_timer,SIGNAL(timeout()),SLOT(consultarOtroEquipo()));
}

MainWindow::~MainWindow()
{   
    limpiar();
    delete ui;
}

void MainWindow::limpiar()
{
    ui->btnCopy->setEnabled(false);
    ui->btnDetener->setEnabled(false);
    ui->btnIniciar->setEnabled(true);
    ui->ledGW->setEnabled(true);
    ui->ledMAC->setEnabled(true);
    ui->ledPE->setEnabled(true);
    ui->ledPWD->setEnabled(true);
    ui->ledUsuario->setEnabled(true);

    lstEquipospos = -1;
    stop = false;

    actualizarInterfaces_timer->stop();

    foreach (Equipo *equipo, lstEquipos)
        delete equipo->qry;
    qDeleteAll(lstEquipos);

    delete model;
    lstEquipos.clear();
    ui->lblStatus->setText("");
    loadingIcon->stop();
}

void MainWindow::on_btnIniciar_clicked()
{
    if ( ui->ledMAC->text().isEmpty() ||
         ui->ledPE->text().isEmpty() ||
         ui->ledPWD->text().isEmpty() ||
         ui->ledUsuario->text().isEmpty() )
    {
        QMessageBox::critical(this,"Error","Favor de ingresar toda la información");
        return;
    }

    ui->ledMAC->setText( ui->ledMAC->text().simplified() );
    ui->ledPE->setText( ui->ledPE->text().simplified() );
    ui->ledPWD->setText( ui->ledPWD->text().simplified() );
    ui->ledUsuario->setText( ui->ledUsuario->text().simplified() );
    ui->ledGW->setText( ui->ledGW->text().simplified() );

    ui->btnIniciar->setEnabled(false);
    ui->btnDetener->setEnabled(true);
    ui->ledGW->setEnabled(false);
    ui->ledMAC->setEnabled(false);
    ui->ledPE->setEnabled(false);
    ui->ledPWD->setEnabled(false);
    ui->ledUsuario->setEnabled(false);

    siguienteIP = ui->ledPE->text();
    descubrirSiguienteEquipo();
}

void MainWindow::on_btnDetener_clicked()
{
    stop=true;
    QTimer::singleShot(1000,this,SLOT(limpiar()));
}

void MainWindow::on_queries_lastCommand(Queries* qry, QString cmd)
{
    ui->lblStatus->setText("Consultando: "+qry->hostName() + ": "+ cmd);
    loadingIcon->start();
}

void MainWindow::descubrirSiguienteEquipo()
{      
    if ( stop )
        return;

    qDebug() << "iniciamos" << ui->ledPE->text() << ui->ledGW->text() << ui->ledMAC->text();

    Queries *qry = new Queries( siguienteIP,this );
    unsigned int opciones;    
    opciones = Queries::Connect |
            Queries::Platform |
            Queries::EquipmentNeighbors |
            Queries::MacAddress |
            Queries::PortChannel;

    qry->setOptions(opciones);
    qry->setGW( ui->ledGW->text() );

    QueriesConfiguration::instance()->mapQueries.insert("MAC_MAC",ui->ledMAC->text());

    connect(qry,SIGNAL(finished(Queries*)),SLOT(descubrirSiguienteEquipoProccess(Queries*)));
    connect(qry,SIGNAL(lastCommand(Queries*,QString)),SLOT(on_queries_lastCommand(Queries*,QString)));
    qry->start();
}

void MainWindow::descubrirSiguienteEquipoProccess(Queries *qry)
{
    if ( stop )
        return;

    loadingIcon->stop();
    ui->lblStatus->clear();

    if ( qry->isConnected() )
    {
        QString interfazSalida = qry->macsQuery->macInterfaceLearnedFrom( ui->ledMAC->text() );

        if ( interfazSalida.isEmpty() )
        {
            QMessageBox::critical(this,"Error","No se encuentra la MAC "+ui->ledMAC->text()+" en el equipo con IP: "+
                                  qry->ip());
            on_btnDetener_clicked();
            return;
        }

        Equipo *e = new Equipo;
        e->nombre = qry->hostName();
        e->IP = qry->ip();
        e->interfacesentrada = siguientesInterfacesEntrada;

        if ( siguientesInterfacesEntrada.size() > 0 )
        {
            //verificamos si la interfaz de entrada pertenece a un Po y lo agregamos al listado
            QString po = qry->portChannelInfoQuery->interfaceToPortChannel( siguientesInterfacesEntrada.at(0) );
            if ( !e->interfacesentrada.contains(po) )
                e->interfacesentrada.append(po);
        }

        e->interfacessalida = qry->portChannelInfoQuery->portChannelToInterfaces( interfazSalida );

        //verificamos si la interfazSalida no esta en el listado, si no esta es por que es un Po y agregaron sus miembros
        //agregamos tambien el Po
        if ( !e->interfacessalida.contains(interfazSalida) )
            e->interfacessalida.append( interfazSalida );

        e->qry = qry;

        siguienteIP.clear();
        siguientesInterfacesEntrada.clear();

        foreach (QString interfazsalida, e->interfacessalida)
        {
            foreach (SEquipmentNeighborsInfo *vecino, qry->equipmentNeighborsInfo())
            {
                if ( vecino->interfazestesalida ==  interfazsalida )
                {
                    siguienteIP = vecino->ip;
                    siguientesInterfacesEntrada.append( vecino->interfazotroentrada );
                    break;
                }
            }
        }

        if ( lstEquipos.size() == 0 && siguienteIP.isEmpty() )
        {
            QMessageBox::critical(this,"Siguiente interfaz","Primer equipo no tiene información de EquipmentNeighbors en el downlink.\n\nFavor de ingresar el siguiente equipo para continuar desde ahi.");
            on_btnDetener_clicked();
            return;
        }

        lstEquipos.append( e );
        qry->clear();
    }
    else
    {
        QMessageBox::critical(this,"Error","No se pudo conectar al equipo con la IP: "+qry->ip());
        on_btnDetener_clicked();
        return;
    }

    if ( !siguienteIP.isEmpty() )
        descubrirSiguienteEquipo(); //se continua descubriendo equipos ya que hay informacion de EquipmentNeighbors
    else
    {
        //ya no se conoce informacion de EquipmentNeighbors por lo que se empieza el ciclo de consultas para actualizar datos
        ui->btnCopy->setEnabled(true);
        consultarOtroEquipo();
    }
}

void MainWindow::consultarOtroEquipo()
{
    if ( stop )
        return;

    if ( lstEquipospos < lstEquipos.size()-1 )   
        lstEquipospos++;    
    else    
    {
        lstEquipospos=-1;
        mostrarTabla();
        actualizarInterfaces_timer->start();
        return;
    }

    equipo = lstEquipos.at( lstEquipospos );    

    unsigned int opciones;
    opciones = Queries::InterfaceInformation;
    equipo->qry->setOptions( opciones );

    QStringList lstInterfaces = equipo->interfacesentrada + equipo->interfacessalida;
    QueriesConfiguration::instance()->mapQueries.insert("InterfaceInfo_Interfaces",lstInterfaces.join(","));

    connect(equipo->qry,SIGNAL(finished(Queries*)),SLOT(consultarInterfazProcess(Queries*)));
    connect(equipo->qry,SIGNAL(lastCommand(Queries*,QString)),SLOT(on_queries_lastCommand(Queries*,QString)));
    equipo->qry->start();
}

void MainWindow::consultarInterfazProcess(Queries *qry)
{   
    if ( stop )
        return;

    loadingIcon->stop();
    ui->lblStatus->clear();

    consultarOtroEquipo();
}

void MainWindow::mostrarTabla()
{
    QList<Queries*> lstQueries;
    foreach (Equipo *e, lstEquipos)
        lstQueries.append( e->qry );

    if ( !model )
    {
        model = new InterfaceInfoQueriesModel(this);
        model->setQueries( lstQueries );
        ui->tableView->setModel( model );
    }

    model->updateModel();
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::on_btnCopy_clicked()
{
    QString salida;
    foreach (Equipo *e, lstEquipos)
    {
        if ( e->interfacesentrada.size() > 0 )
        {
            if ( e->interfacesentrada.size() > 1 )
                salida.append("(");
            foreach (QString i, e->interfacesentrada )
                salida.append( i+"," );
            salida.remove( salida.size()-1,1 );
            if ( e->interfacesentrada.size() > 1 )
                salida.append(") ");
            else
                salida.append(" ");
        }

        salida.append( e->nombre+"("+e->IP+") " );

        if ( e->interfacessalida.size() > 1 )
            salida.append("(");
        foreach (QString i, e->interfacessalida )
            salida.append( i+"," );
        salida.remove( salida.size()-1,1 );
        if ( e->interfacessalida.size() > 1 )
            salida.append(") ");
        else
            salida.append(" ");
        salida.append("-->\n");
    }
    salida.append(" Destino");

    QApplication::clipboard()->setText(salida);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QApplication::clipboard()->setText(ui->tableView->currentIndex().sibling(index.row(),0).data(0).toString());
}
