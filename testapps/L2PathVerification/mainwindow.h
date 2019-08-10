#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidget>
#include <QMovie>
#include <QFile>

#include "queries.h"
#include "interfaceinfoqueriesmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnIniciar_clicked();
    void on_btnDetener_clicked();
    void on_queries_lastCommand(Queries*, QString);

    void consultarOtroEquipo();
    void descubrirSiguienteEquipoProccess(Queries *qry);
    void consultarInterfazProcess(Queries *qry);

    void limpiar();    
    void on_btnCopy_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    struct Equipo
    {
        QString IP;
        QString nombre;
        QStringList interfacesentrada;
        QStringList interfacessalida;
        Queries* qry;        
    };

    QMovie *loadingIcon;

    QList<Equipo*> lstEquipos;
    Equipo* equipo;
    QTimer *actualizarInterfaces_timer;
    short int lstEquipospos;    
    bool stop;
    InterfaceInfoQueriesModel *model;

    qint64 _bw;

    QList<QTableWidgetItem*> lstItems;

    QString siguienteIP;
    QStringList siguientesInterfacesEntrada;

    void descubrirSiguienteEquipo();
    void mostrarTabla();
};

#endif // MAINWINDOW_H
