#include "orderconfirmation.h"
#include "ui_orderconfirmation.h"

orderConfirmation::orderConfirmation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::orderConfirmation),
    mStackedWidget(nullptr),
    mConditionWidget(nullptr),
    mOrderWidget(nullptr),
    selectedSteelIDs(),
    selectedSteelTypes(),
    selectedSteelWeights(),
    selectedSteelPrioritys(),
    steelInitialPositions(),
    steelGoalPositions()
{
    ui->setupUi(this);
    initPage();
    //设置表格视图
    ui->orderTableView->setModel(&orderModel);
    ui->orderTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orderTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orderTableView->setSelectionMode(QAbstractItemView::MultiSelection);
    orderModel.setHorizontalHeaderLabels({"货物名称","货物重量","起始货场","目标货场"});
    ui->conditionTableView->setModel(&conditionModel);
    ui->conditionTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->conditionTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->conditionTableView->setSelectionMode(QAbstractItemView::MultiSelection);
    conditionModel.setHorizontalHeaderLabels({"指定对象ID","指定对象","特殊要求","目标对象ID","目标对象"});
    for (int row = 0; row < conditionModel.rowCount(); ++row)
    {
        for (int column = 1; column <= 2; ++column)
        {
            QStandardItem* item = conditionModel.item(row, column);
            if (item) {
                item->setEditable(false);
            }
        }
    }
    buildSteelMap();
}

orderConfirmation::~orderConfirmation()
{
    delete mStackedWidget;
    delete mConditionWidget;
    delete mOrderWidget;
    for (int i = 0; i < transitionList.size(); ++i)
    {
        delete transitionList[i];
    }
    delete ui;
}
void orderConfirmation::generateLTLPy()
{
//生成LDL的.py文件
    //设置.py文件的文件路径pyFilePath
    QFile pyFile(pyFilePath);
    QFile sharePyFile(sharePyFilePath);
    //在.py文件中写入ltl语句
    if (pyFile.open(QIODevice::WriteOnly | QIODevice::Text) &&
        sharePyFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        //清空文件
        pyFile.resize(0);
        QTextStream stream(&pyFile);
        QTextStream shareStream(&sharePyFile);
        // 将LTL约束语句写入本地文件
        stream << "from " << "ltlf2dfa.parser.ltlf import LTLfParser";
        stream << "\n";
        stream << "parser" << "=" << "LTLfParser()" ;
        stream << "\n";
        stream << "formula_str" << "=" << "\"" << ltlStatement << "\"";
        stream << "\n";
        stream << "formula" << "=" << "parser(formula_str)";
        stream << "\n";
        stream << "print(formula)";
        stream << "\n";
        stream << "dfa" << "=" << "formula.to_dfa()";
        stream << "\n";
        stream << "print(dfa)";

        shareStream << "from " << "ltlf2dfa.parser.ltlf import LTLfParser";
        shareStream << "\n";
        shareStream << "parser" << "=" << "LTLfParser()" ;
        shareStream << "\n";
        shareStream << "formula_str" << "=" << "\"" << ltlStatement << "\"";
        shareStream << "\n";
        shareStream << "formula" << "=" << "parser(formula_str)";
        shareStream << "\n";
        shareStream << "print(formula)";
        shareStream << "\n";
        shareStream << "dfa" << "=" << "formula.to_dfa()";
        shareStream << "\n";
        shareStream << "print(dfa)";

        pyFile.close();
        sharePyFile.close();
        qDebug() << "文件内容已保存到本地";
    }
}
void orderConfirmation::on_cdtConformationPushButton_clicked()
{
//在UI中点击确认按钮
    //获取表格视图中有特殊要求的钢材和要求的内容
    int lastRow = conditionModel.rowCount();
    for(int row = 0 ;row < lastRow ; ++row)
    {
        //获取特殊要求和目标钢材
        QString specifyID = specifyIDComboBoxArray[row]->currentText();
        QString specifySteel = specifySteelComboBoxArray[row]->currentText();
        QString specialRequirements = specialRequirementsComboBoxArray[row]->currentText();
        QString targetID = targetIDComboBoxArray[row]->currentText();
        QString targetSteel = targetSteelComboBoxArray[row]->currentText();
        if(!specifyID.isEmpty() && !specifySteel.isEmpty() && !specialRequirements.isEmpty() && !targetID.isEmpty()&& !targetSteel.isEmpty())
        {
            if(specialRequirements == "before")
            {
                ltlStatement.append("F( unload_");
                ltlStatement.append(specifySteel + "_" +specifyID);
                ltlStatement.append(" U ( unload_");
                ltlStatement.append(targetSteel+ "_" +targetID);
                ltlStatement.append(" ))");
            }
            else if (specialRequirements == "after")
            {
                ltlStatement.append("F( load_");
                ltlStatement.append(targetSteel+ "_" +targetID);
                ltlStatement.append(" U ( load_");
                ltlStatement.append(specifySteel+ "_" +specifyID);
                ltlStatement.append(" ))");
            }
        }
    }
//.py > .dot > .pddl
    generateLTLPy();
    qDebug() << "generate .py" << '\t';
    invokeLTL();
    qDebug() << "generate .dot" << '\t';
    dotGeneratesPDDL();
    qDebug() << "generate PDDL" << '\t';
    close();
}
QStringList orderConfirmation::replaceCondition(QString originalString)
{
    //替换originalString中&|~为符合PDDL语法的resultString
    QStringList substrings;
    QString resultString;
    resultString.append("( ");
    if(!originalString.contains("&") && !originalString.contains("|") && !originalString.contains("~"))
    {
        //没有&|~的表达式返回 originalString
        resultString.append(originalString);
    }
    else if (originalString.contains("&") && originalString.contains("|") &&originalString.contains("~"))
    {
        //含有&的表达式返回(and
        QStringList andConditions = originalString.split("&", Qt::SkipEmptyParts);
        QString andResult = " and ";
        foreach(QString andStr, andConditions)
        {
            if(!andStr.contains("(") && !andStr.contains(")"))
            {
                andStr.prepend("(");
            }
            if(andStr.contains("|"))
            {
                QStringList orConditions = andStr.split("|", Qt::SkipEmptyParts);
                QString orResult = "(or ";
                foreach (QString orStr, orConditions)
                {
                    orStr.replace("|", " )(");
                    if (orStr.contains("~"))
                    {
                        orStr.replace("~", "  not (");
                    }
                    orResult += orStr.trimmed() + " ";
                }
                orResult += ")";
                andResult += orResult;
                qDebug() << andResult;
            }
            else
            {
                andResult += andStr.trimmed() + " ";
            }
        }
        andResult += ")";
        resultString = andResult.trimmed();
        qDebug() << "andStr:" << resultString;
    }

    else if (originalString.contains("|")&& originalString.contains("~"))
    {
        QStringList orConditions = originalString.split("|", Qt::SkipEmptyParts);
        QString orResult = "(or ";
        foreach(QString orStr, orConditions)
        {
            if (orStr.contains("~"))
            {
                orStr.replace("~", " not (");
            }
            orResult += orStr.trimmed() + " ";
        }
        orResult += ")";
        resultString = orResult.trimmed();
        qDebug() << "orStr:" << resultString;

    }
    else if (originalString.contains("~"))
    {
        //含有~ 返回 not(originalString)
        originalString.replace("~", " not (");
        resultString.append(originalString);
        resultString.append(" )");
        qDebug() << "noneStr:" << resultString;

    }
    resultString.append(" )");
    substrings.append(resultString);
    return substrings;
}

void orderConfirmation::invokeLTL()
{
    //用QProcess类在Windows中用ssh连接linux，让linux访问共享文件夹用.py文件生成.dot文件
    // 设置要执行的 Python 脚本路径
    QString scriptPath = "D:/QtProject/TranspotationSystem/outputFile/ssh.py";
    // 创建 QProcess 对象
    QProcess process;
    // 设置要执行的命令
    QString command = "python";
    QStringList arguments;
    arguments << scriptPath;

    // 启动命令
    process.start(command, arguments);
    // 等待命令执行完成
    process.waitForFinished();
    // 读取输出结果
    QByteArray output = process.readAllStandardOutput();
    QByteArray error = process.readAllStandardError();
    // 打印输出和错误信息
    qDebug() << "Output:" << output;
    qDebug() << "Error:" << error;
}
void orderConfirmation::dotGeneratesPDDL()
{
//将.dot文件生成.pddl文件order.pddl（以追加方式写入）
    //(\\d+) 匹配一个或多个数字，匹配第一个状态
    //\\s*->\\s* 匹配箭头 ->，允许在箭头两侧有零个或多个空格
    //(\\d+) 匹配一个或多个数字，匹配第二个状态
    //\"([^\"]+)\"匹配双引号中所有的内容
    //\"表示"的转义
    QRegularExpression transitionRegex("(\\d+)\\s*->\\s*(\\d+)\\s+\\[label=\"([^\"]+)\"];");
    QRegularExpression nodeRegex("node\\s+\\[shape\\s*=\\s*\\w+\\s*];\\s*\\d+\\s*;");
    transitionRegex.setPatternOptions(QRegularExpression::MultilineOption);
    nodeRegex.setPatternOptions(QRegularExpression::MultilineOption);

    QFile file(dotFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开文件";
        return;
    }
    QTextStream in(&file);
    QString line;

    while(!in.atEnd())
    {
        line = in.readLine();
        QRegularExpressionMatch transtionMatch = transitionRegex.match(line);
        QRegularExpressionMatch nodeMatch = nodeRegex.match(line);

        if(transtionMatch.hasMatch())
        {
            Transtion *t = new Transtion;
            t->firstState = transtionMatch.captured(1);
            t->secondState = transtionMatch.captured(2);
            QString condition = transtionMatch.captured(3);
            qDebug() << "dot文件中匹配到的条件condition"<< condition << '\n';

            t->conditions = replaceCondition(condition);
            transitionList.append(t);
            qDebug() << "dot文件中进行修改后匹配到的条件condition"<< t->conditions << '\n';
        }
        //node匹配没写
        if(nodeMatch.hasMatch())
        {
        }
    }
    //读取模板PDDLFile中内容写入newPDDLFile
    QFile PDDLFile(PDDLFilePath);
    if (!PDDLFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open PDDL file.";
        return;
    }
    QByteArray PDDLData = PDDLFile.readAll();
    QFile newPDDLFile("D://MetricFF//TransportationSystem_order_"+QString::number(selectedSteelIDs[0])+"_pro.pddl");
    if (!newPDDLFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Failed to open new PDDL file.";
        return;
    }

    newPDDLFile.write(PDDLData);
    PDDLFile.close();

    //解析dot文件生成新的action以追加的方式写在newPDDLFile
        QTextStream stream(&newPDDLFile);
        foreach (Transtion *t, transitionList)
        {
            stream << "(:action transition-";
            stream << t->firstState;
            stream << "-to-";
            stream << t->secondState;
            stream << "\n";
            stream << "\t:parameters (?s - steel)\n";
            stream << "\t:precondition (and \n";
            stream << "\t\t(automata ?s)\n";
            stream << "\t\t(q";
            stream << t->firstState;
            stream << " ?s)\n";
            foreach (QString str, t->conditions)
            {
                QString pddlCondition;
                pddlCondition = changeStr(str);
                stream << pddlCondition;
            }
            stream << "\n)\n";
            stream << "\t:effect (and \n";
            stream << "\t\t(not (automata ?s))\n";
            stream << "\t\t(world)\n";
            stream << "\t\t(not (q";
            stream << t->firstState;
            stream << " ?s))\n";
            stream << "\t\t(q";
            stream << t->secondState;
            stream << " ?s)\n";
            stream << "\t )\n";
            stream << ")\n";
        }
        stream << ")";
        stream.flush();
        newPDDLFile.close();
}
//将替换过&|~的语句进一步改写 比如unload_bar写为not (order-completed bar)
QString orderConfirmation::changeStr(QString originalString)
{
    QString pddlCondition;
    if(originalString.contains("true"))
    {
        pddlCondition.append("\n");
    }
    else if (originalString.contains("unload"))
    {
        QRegularExpression regex("unload_(\\w+)");
        QRegularExpressionMatch match = regex.match(originalString);

        if (match.hasMatch()) {
            // 获取匹配的内容并进行替换
            QString replacedString = originalString.replace(regex, "not (order-completed \\1 )");
            pddlCondition.append(replacedString);
        }
    }
    else if (originalString.contains("load"))
    {
        QRegularExpression regex("load_(\\w+)");
        QRegularExpressionMatch match = regex.match(originalString);

        if (match.hasMatch()) {
            // 获取匹配的内容并进行替换
            QString replacedString = originalString.replace(regex, " order-completed \\1 ");
            pddlCondition.append(replacedString);
        }
    }
    return pddlCondition;
}

void orderConfirmation::on_conditionPushButton_clicked()
{
    mStackedWidget->setCurrentWidget(mConditionWidget);
}
void orderConfirmation::on_addButton_clicked()
{
//添加条件按钮
    int row = conditionModel.rowCount();
    conditionModel.insertRow(row);
    showCondition(row);
}
void orderConfirmation::showCondition()
{
//获取Condition下拉框中文本内容（读取表格视图中第一行文本内容）
    int lastRow = conditionModel.rowCount();
    for(int row = 0;row < lastRow ;++row)
    {
        for(int conlumn = 0;conlumn < 1 ;conlumn++)
        {
            QComboBox* specifySteelCmb = new QComboBox();
            QStringList specialRequirementsList;
            specialRequirementsList.append("");
            specialRequirementsList.append(selectedSteelTypes);
            specifySteelCmb->addItems(specialRequirementsList);
            specifySteelCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), specifySteelCmb);
            // 将 QComboBox 指针添加到 QVector 中
            targetSteelComboBoxArray.append(specifySteelCmb);
        }

        for(int conlumn = 1;conlumn < 2 ;conlumn++)
        {
            QComboBox* specialRequirementsCmb = new QComboBox();
            QStringList specialRequirements;
            specialRequirements.append("");
            specialRequirements.append("before");
            specialRequirements.append("after");
            specialRequirementsCmb->addItems(specialRequirements);
            specialRequirementsCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), specialRequirementsCmb);
            // 将 QComboBox 指针添加到 QVector 中
            specialRequirementsComboBoxArray.append(specialRequirementsCmb);
        }
        QString originalSteel = orderModel.index(row, 0).data().toString();
        QStringList removeItselfSteel;
        for(int conlumn = 2;conlumn < 3 ;conlumn++)
        {
            removeItselfSteel.append("");
            for (const QString &steel : selectedSteelTypes)
            {
                if (steel != originalSteel)
                {
                    removeItselfSteel.append(steel);
                }
            }
            QComboBox* steelGoalPositionCmb = new QComboBox();
            steelGoalPositionCmb->addItems({removeItselfSteel});
            steelGoalPositionCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), steelGoalPositionCmb);
            // 将 QComboBox 指针添加到 QVector 中
            specifySteelComboBoxArray.append(steelGoalPositionCmb);
        }
    }
}
void orderConfirmation::showCondition(int row)
{
//在表格视图中添加新一行时调用该函数用来设置下拉框文本内容
    int lastRow = conditionModel.rowCount();
    for(int i = row;i < lastRow ;++i)
    {
        for(int conlumn = 0;conlumn < 1 ;conlumn++)
        {
            QComboBox* specifyIDCmb = new QComboBox();
            QStringList specialIDsList;
            specialIDsList.append(" ");
            for (int id : selectedSteelIDs)
            {
                specialIDsList.append(QString::number(id));
            }
            specifyIDCmb->addItems(specialIDsList);
            specifyIDCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), specifyIDCmb);
            // 将 QComboBox 指针添加到 QVector 中
            specifyIDComboBoxArray.append(specifyIDCmb);
        }
        for(int conlumn = 1;conlumn < 2 ;conlumn++)
        {
            QComboBox* specifySteelCmb = new QComboBox();
            QStringList specialRequirementsList;
            specialRequirementsList.append("");
            specialRequirementsList.append(selectedSteelTypes);
            specifySteelCmb->addItems(specialRequirementsList);
            specifySteelCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), specifySteelCmb);
            // 将 QComboBox 指针添加到 QVector 中
            specifySteelComboBoxArray.append(specifySteelCmb);
        }

        for(int conlumn = 2;conlumn < 3 ;conlumn++)
        {
            QComboBox* specialRequirementsCmb = new QComboBox();
            QStringList specialRequirements;
            specialRequirements.append("");
            specialRequirements.append("before");
            specialRequirements.append("after");
            specialRequirementsCmb->addItems(specialRequirements);
            specialRequirementsCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), specialRequirementsCmb);
            // 将 QComboBox 指针添加到 QVector 中
            specialRequirementsComboBoxArray.append(specialRequirementsCmb);
        }
        for(int conlumn = 3;conlumn < 4 ;conlumn++)
        {
            QComboBox* tragetIDCmb = new QComboBox();
            QStringList tragetIDsList;
            tragetIDsList.append(" ");
            for (int id : selectedSteelIDs)
            {
                tragetIDsList.append(QString::number(id));
            }
            tragetIDCmb->addItems(tragetIDsList);
            tragetIDCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), tragetIDCmb);
            // 将 QComboBox 指针添加到 QVector 中
            targetIDComboBoxArray.append(tragetIDCmb);
        }
        for(int conlumn = 4;conlumn < 5 ;conlumn++)
        {
            QComboBox* steelGoalPositionCmb = new QComboBox();
            QStringList tragetRequirementsList;
            tragetRequirementsList.append(" ");
            tragetRequirementsList.append(selectedSteelTypes);
            steelGoalPositionCmb->addItems({tragetRequirementsList});
            steelGoalPositionCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->conditionTableView->setIndexWidget(conditionModel.index (row,conlumn), steelGoalPositionCmb);
            // 将 QComboBox 指针添加到 QVector 中
            targetSteelComboBoxArray.append(steelGoalPositionCmb);
        }
    }
}

void orderConfirmation::initPage()
{
    mStackedWidget = ui->stackedWidget;
    mConditionWidget = ui->conditionWidget;
    mOrderWidget = ui->orderWidget;
    // 添加界面
    mStackedWidget->addWidget(mOrderWidget);
    mStackedWidget->addWidget(mConditionWidget);

    // 设定初始界面
    mStackedWidget->setCurrentWidget(mOrderWidget);
}
void orderConfirmation::buildSteelMap()
{
    steelIDToType.clear();
    for (int i = 0; i < selectedSteelIDs.size(); ++i) {
        int steelID = selectedSteelIDs.at(i);
        QString steelType = selectedSteelTypes.at(i);
        steelIDToType.insert(steelID, steelType);
    }
}
void orderConfirmation::showOrder()
{
    for(int i = 0; i < selectedSteelIDs.size(); i++)
    {
        //按行设置订单中的钢材资料
        QList<QStandardItem*> rowItems;
        QStandardItem* orderSteelType = new QStandardItem(selectedSteelTypes[i]);
        QStandardItem* orderSteelWeight = new QStandardItem(QString::number(selectedSteelWeights[i]));
        QStandardItem* orderInitialPositon = new QStandardItem(steelInitialPositions[i]);
        QStandardItem* orderGoalPosition = new QStandardItem(steelGoalPositions[i]);
        rowItems << orderSteelType << orderSteelWeight << orderInitialPositon << orderGoalPosition << new QStandardItem("");
        orderModel.appendRow(rowItems);
    }
    queryYard();

}
void orderConfirmation::sendID(QList<int> orderSteelIDs)
{
    //其他类向orderConfirmation类发送选中的钢材编号
    selectedSteelIDs = orderSteelIDs;

}
void orderConfirmation::sendType(QStringList orderSteelTypes)
{
    //其他类向orderConfirmation类发送选中的钢材类型
    selectedSteelTypes = orderSteelTypes;
}
void orderConfirmation::sendWeight(QList<double> orderSteelWeights)
{
    //其他类向orderConfirmation类发送选中的钢材质量
    selectedSteelWeights = orderSteelWeights;
}
void orderConfirmation::sendPriority(QList<int> orderSteelPrioritys)
{
    //其他类向orderConfirmation类发送选中的钢材优先级
    selectedSteelPrioritys = orderSteelPrioritys;
}
void orderConfirmation::sendInitialPosition(QStringList orderSteelInitialPositions)
{
    //其他类向orderConfirmation类发送选中的钢材的初始位置
    steelInitialPositions = orderSteelInitialPositions;
}
void orderConfirmation::sendGoalPosition(QStringList orderSteelGoalPositions )
{
    //其他类向orderConfirmation类发送选中的钢材目的地
    steelGoalPositions = orderSteelGoalPositions;
}
void orderConfirmation::sendYardItem(QStringList sentYardItems)
{
    //其他类向orderConfirmation类发送货场信息
    yardItems = sentYardItems;
}
void orderConfirmation::queryYard()
{
    //查询所有货场位置
    QSqlQuery yardQuery;
    QString sql = "SELECT *FROM FreightYard";
    if(yardQuery.exec(sql))
    {
        while(yardQuery.next())
        {
            QString yardName = yardQuery.value("yardName").toString();
            yardItems.append(yardName);
        }
    }
}
