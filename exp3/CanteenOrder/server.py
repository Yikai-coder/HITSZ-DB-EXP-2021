from flask import Flask, render_template, request, send_from_directory, jsonify, redirect, url_for
import requests
import hashlib
import random
import time
import json
# from flask_sqlalchemy import SQLAlchemy 
# from sqlalchemy.orm import sessionmaker
# from sqlalchemy.ext.declarative import declarative_base
import pymysql
from pymysql.converters import escape_string
from gevent import pywsgi
import datetime


app=Flask(__name__)
# app.config['SECRET_KEY'] ='hard to guess'
# app.config['SQLALCHEMY_DATABASE_URI']='mysql://root:@localhost:3306/canteen_order_db' #这里登陆的是root用户，要填上自己的密码，MySQL的默认端口是3306，填上之前创建的数据库名text1
# app.config['SQLALCHEMY_COMMIT_ON_TEARDOWN']=True #设置这一项是每次请求结束后都会自动提交数据库中的变动
# app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = True
# db = SQLAlchemy(app) #实例化
conn = pymysql.connect(host='localhost', port=3306, user='root', passwd='', db='canteen_order_db', charset="utf8mb4")
conn.autocommit(True)



@app.route("/")
@app.route("/index")
def jump_to_index():
     # 考虑使用flask中send_file来返回静态页面
     return render_template("index.html")

@app.route("/login", methods=["POST"])
def checkUserInfo():
     # 获取从前端传来的用户名和密码
     userName = request.form['userName']
     passwd = request.form['passwd']
     # print(userName, passwd)
     # 查询数据库
     cur = conn.cursor()
     cur.execute("select * from userInfo where account={} and password={}".format(userName, passwd))
     result = cur.fetchall()
     print(result)    # result=[account, passwd, characterVal]
     # 根据返回的结果不同返回不同的数据
     if(result):
          data = {'code': 0, 'account': result[0][0], 'passwd': result[0][1], 'characterVal': result[0][2]}
     else:
          data={'code':-1, 'message':'No such user!'}
     cur.close()
     return data
     # return 111

@app.route("/signup")
def jump_to_signup():
     return render_template("signup.html")

@app.route("/signup_submit",  methods=["POST"])
def signup_submit():
     account = request.form['account']
     name = request.form['name']
     passwd = request.form['passwd']
     character = request.form['character']
     print(character, len(character))
     print(repr(character), len(repr(character)))
     cur = conn.cursor()
     if(character=="消费者"):
          cur.execute("insert into consumerInfo(account, password, name, state, characterVal) values({}, {}, {}, '有效', {})".format(account, repr(passwd), repr(name), repr(character)))
     elif(character=="商户"):
          cur.execute("insert into stallKeeperInfo(account, password, name, state, characterVal) values({}, {}, {}, '有效', {})".format(account, repr(passwd), repr(name), repr(character)))
     elif(character=="经理"):
          cur.execute("insert into managerInfo(account, password, name, state, characterVal) values({}, {}, {}, '有效', {})".format(account, repr(passwd), repr(name), repr(character)))
     elif(character=="管理员"):
          cur.execute("insert into administerInfo(account, password, name, state, characterVal) values({}, {}, {}, '有效', {})".format(account, repr(passwd), repr(name), repr(character)))
     else:
          return "-1"
     result = cur.fetchall()
     cur.close()
     return "0"

@app.route("/consumer_page")
def jump_to_consumerPage():
     return render_template("consumer_page.html")

@app.route("/stall_keeper_page")
def jump_to_stallKeeperPage():
     return render_template("stall_keeper_page.html")

@app.route("/stall_keeper_menu")
def jump_to_stallKeeperMenu():
     return render_template("stall_keeper_menu.html")

@app.route("/canteen_manage")
def jump_to_canteenManagePage():
     return render_template("canteen_manage.html")

@app.route("/getCanteenInfo", methods=["POST"])
def return_canteenInfo():
     cur = conn.cursor()
     cur.execute("select canteenID, canteenName from canteen")
     result = cur.fetchall()
     canteenNames=[]
     canteenIDs=[]
     for item in result:
          canteenIDs.append(item[0])
          canteenNames.append(item[1])
     data={'canteenIDs': canteenIDs, 'canteenNames': canteenNames}
     print(data)
     cur.close()
     return data

@app.route("/menu")
def jump_to_menu():
     return render_template("menu.html")

@app.route("/getStallsFromDB", methods=["POST"])
def return_stalls_from_db():
     canteenID = request.form["canteenID"]
     print("canteenID:"+canteenID)
     cur = conn.cursor()
     cur.execute("select stallID, stallName, menuID, stallImageUrl\
                  from stall\
                  where stall.canteenID={} and state='有效'".format(canteenID))
     result = cur.fetchall()
     stallRanks=[]
     for item in result:
          cur.execute("select avg(stallRank) from comment, indent where indent.stallID={} and indent.indentID=comment.indentID".format(item[0]))
          stallRanks.append(cur.fetchone()[0])
     data = {'stalls': result, 'stallRanks': stallRanks}
     print(data)
     cur.close()
     return data

@app.route("/getDishesFromDB", methods=["POST"])
def return_dishes_from_db():
     menuID = request.form["menuID"]
     cur = conn.cursor()
     cur.execute("select dish.dishID, dish.dishName, dish.price\
                    from dish, menu_dish \
                    where menu_dish.menuID = {} and dish.dishID=menu_dish.dishID".format(menuID))
     result = cur.fetchall()
     data = {'dishes': result}
     cur.close()
     return data

@app.route("/details")
def jump_to_details():
     return render_template("details.html")



@app.route("/getDishInfo", methods=["POST"])
def return_dish_info_from_db():
     dishID = request.form['dishID']
     cur = conn.cursor()
     cur.execute("select dish.dishName, dish.price, dish.dishDescription, dish.dishImageUrl\
                    from dish\
                    where dish.dishID={}".format(dishID))
     dishResult = cur.fetchone()
     cur.execute("select AVG(dishRank) from comment, indent where indent.dishID={} and comment.indentID=indent.indentID".format(dishID))
     dishRank = cur.fetchone()[0]
     data = {'code': 1, 'dishResult': dishResult, 'dishRank': dishRank}
     cur.close()     
     return data

@app.route("/order_confirm")
def jump_out_confirm_page():
     return render_template("order_confirm.html")

@app.route("/manager_page")
def jump_to_manager_page():
     return render_template("manager_page.html")


@app.route("/submitOrder", methods=["POST"])
def submit_order():
     dishID = request.form['dishID']
     account = request.form['account']
     remark = request.form['remark']
     amount = request.form['amount']
     destination = request.form['destination']
     cur = conn.cursor()
     cur.execute("select consumerID from consumerInfo where account={}".format(account))
     consumerID = cur.fetchone()[0]
     cur.execute("select stallKeeperID, stallID\
                  from menu_dish, stall\
                  where menu_dish.dishID={} and stall.menuID=menu_dish.menuID".format(dishID))
     result = cur.fetchone()
     stallKeeperID = result[0]
     stallID=result[1]
     cur.execute("insert into indent(stallKeeperID, consumerID, state, amount, buildTime, remark, destination, stallID, dishID)\
                     values({}, {}, '未完成', {}, now(), {}, {}, {}, {})".format(stallKeeperID, consumerID, amount, repr(remark), repr(destination), stallID, dishID))
     cur.close()

     return "0"




@app.route("/getConsumerOrder", methods=["POST"])
def return_consumer_orders():
     account = request.form['account']
     cur = conn.cursor()
     cur.execute("select distinct consumerInfo.consumerID, dish.dishID, dish.dishName, indent.indentID, indent.amount, indent.buildTime, indent.state,\
                    stall.stallName, canteen.canteenName\
                    from dish, indent, consumerInfo, stall, menu, menu_dish, stallKeeperInfo, canteen\
                    where consumerInfo.account = '{}' and consumerInfo.consumerID=indent.consumerID and indent.dishID=dish.dishID\
                    and stall.stallID = indent.stallID and stall.canteenID=canteen.canteenID\
                    order by indent.buildTime desc".format(account))
     result = cur.fetchall()
     print(result)
     comments = [];
     for item in result:
          cur.execute("select commentID from comment where indentID={}".format(item[3]))
          commentID = cur.fetchone()
          if(commentID==None):
               comments.append(None)
          else:
               comments.append(commentID)
     data = {'orderList': result, 'comments': comments}
     print(data)
     cur.close()
     return data

@app.route("/getStallOrder", methods=["POST"])
def return_stall_order():
     account = request.form['account']
     cur = conn.cursor()
     cur.execute("select distinct consumerInfo.name, dish.dishID, dish.dishName, indent.indentID, indent.amount, indent.buildTime, indent.state, indent.destination, indent.remark\
                    from dish, indent, consumerInfo, menu, menu_dish, stallKeeperInfo\
                    where  stallKeeperInfo.account = {} and stallKeeperInfo.stallKeeperID=indent.stallKeeperID and indent.dishID=dish.dishID\
                    order by indent.buildTime desc".format(account))
     result = cur.fetchall()
     cur.close();
     data = {'orderList': result}
     return data

@app.route("/confirmOrder", methods=["POST"])
def confirmOrder():
     orderID = request.form['orderID']
     cur = conn.cursor()
     cur.execute("update indent set state='已完成', endTime=Now() where indentID={}".format(orderID));
     cur.close()
     return "0"

@app.route("/submitComment", methods=["POST"])
def submitComment():
     print("Enter submitComment")
     comment = request.form['remark']
     dishRank = request.form['dishRank']     
     stallRank = request.form['stallRank']
     canteenRank = request.form['canteenRank']
     indentID = request.form['indentID']
     account = request.form['account']

     cur = conn.cursor()
     cur.execute("select consumerID from consumerInfo where account={}".format(account))
     consumerID = cur.fetchone()[0]
     cur.execute("insert into comment(appraiserID, comment, dishRank, stallRank, canteenRank, indentID)\
                    values({}, '{}', {}, {}, {}, {})".format(consumerID, comment, dishRank, stallRank, canteenRank, indentID))
     return "0"



@app.route("/getStallMenu", methods=["POST"])
def getStallMenu():
     account = request.form['account']
     cur = conn.cursor()
     cur.execute("select dish.dishID, dishName, dishDescription, dishImageUrl, dish.price\
                    from stallKeeperInfo, stall, menu_dish, dish\
                    where stallKeeperInfo.account={} and stall.stallKeeperID=stallKeeperInfo.stallKeeperID\
                    and stall.menuID=menu_dish.menuID and menu_dish.dishID=dish.dishID".format(account))
     result = cur.fetchall()
     data = {'menu': result}
     cur.close()
     return data

@app.route("/updateMenu", methods=["POST"])
def updateStallMenu():
     menu = request.form['menu']
     account = request.form['account']
     menu = json.loads(menu)
     print(menu)
     cur = conn.cursor()
     dishIDs = []    # 记录更新后的菜单ID，用于删去被删去的菜品

     cur.execute("select stall.menuID from stallKeeperInfo, stall where stallKeeperInfo.account={} and stall.stallKeeperID=stallKeeperInfo.stallKeeperID".format(account))
     menuID = cur.fetchone()[0]
     print(menuID)

     for item in menu:
          dishDescription = item['dishDescription'] if item['dishDescription']!='None' else ''
          dishImageUrl = item['dishImageUrl'] if item['dishImageUrl']!='None' else ''
          # 更新菜品
          if(item['dishID']):
               dishIDs.append(item['dishID'])
               # cur.execute("update dish\
               #                set dishName='{}', dishDescription='{}', dishImageUrl='{}', price={}\
               #                where dishID={}".format(repr(item['dishName']), dishDescription, dishImageUrl, item['dishPrice'], item['dishID']))
               cur.execute("update dish\
                              set dishName='{}', dishDescription='{}', dishImageUrl='{}', price={}\
                              where dishID={}".format(item['dishName'], dishDescription, dishImageUrl, item['dishPrice'], item['dishID']))
          # 添加菜品
          else:
               cur.execute("insert into dish(dishName, dishDescription, dishImageUrl, price) values('{}', '{}', '{}', {})\
                           ".format(item['dishName'], dishDescription, dishImageUrl, item['dishPrice']))
               new_dishID = cur.lastrowid
               dishIDs.append(new_dishID)
               cur.execute("insert into menu_dish values({}, {})".format(new_dishID, menuID))
     # 检查是否有被删除的菜品
     cur.execute("select dishID from menu_dish where menuID={}".format(menuID))
     menu_dishIDs = cur.fetchall()
     for item in menu_dishIDs:
          if(check_list(dishIDs, item[0])==False):
               cur.execute("delete from menu_dish where menuID={} and dishID={}".format(menuID, item[0]))
     cur.close()
     return "0"
     # cur = conn.cursor()

@app.route("/updateStall", methods=["POST"])
def updateCanteenStall():
     stalls = request.form['stalls']
     account = request.form['account']
     print(type(stalls))
     stalls = json.loads(stalls)

     print(type(stalls[0]))
     cur = conn.cursor()
     new_stalls = []    # 记录更新后的菜单ID，用于删去被删去的菜品

     cur.execute("select canteenID from managerInfo, manager_canteen where managerInfo.account={} and manager_canteen.managerID=managerInfo.managerID".format(account))
     canteenID=cur.fetchone()
     canteenID = canteenID[0]
     # cur.execute("select stallKeeperInfo.name, stallKeeperInfo.stallKeeperID, stall.stallName, stall.stallImageUrl\
     #                from stall, stallKeeperInfo\
     #                where stall.canteenID={} and stallKeeperInfo.stallKeeperID=stall.stallKeeperID".format(canteenID))
     # stalls=cur.fetchall()

     for item in stalls:
          print(type(item))
          stallImageUrl = item['stallImageUrl'] if item['stallImageUrl']!='None' else ''
          # 更新档口
          if(item['stallID']):
               new_stalls.append(item['stallID'])
               # 先查找name对应的商户
               cur.execute("select stallKeeperID from stallKeeperInfo where name='{}'".format(item['stallKeeper']))
               stallKeeperID=cur.fetchone()
               if(stallKeeperID!=''):
                    stallKeeperID = stallKeeperID[0]
                    print(stallKeeperID)
                    cur.execute("update stall\
                                   set stallKeeperID={}, stallName='{}', stallImageUrl='{}'\
                                   where stallID={}".format(stallKeeperID, item['stallName'], item['stallImageUrl'], item['stallID']))
               else:
                    return {'code': -1}
          # 添加档口
          else:
               cur.execute("select stallKeeperID from stallKeeperInfo where name='{}'".format(item['stallKeeper']))
               stallKeeperID=cur.fetchone()
               if(stallKeeperID!=''):
                    stallKeeperID = stallKeeperID[0]
                    cur.execute("insert into stall(stallKeeperID, canteenID, state, stallName, stallImageUrl, startTime) values({}, {}, '有效', '{}', '{}', '11:30')".format(stallKeeperID, canteenID, item['stallName'], item['stallImageUrl']))
                    new_stallID = cur.lastrowid
                    new_stalls.append(new_stallID)

               else:
                    return {'code': -1}
     # 检查是否有被删除的档口
     cur.execute("select stallID from stall where canteenID={}".format(canteenID))
     old_stalls = cur.fetchall()
     for item in old_stalls:
          if(check_list(new_stalls, item[0])==False):
               cur.execute("delete from stall where stallID={}".format(item[0]))
     cur.close()
     return "0"

@app.route("/getCanteenIDByManagerAccount", methods=["POST"])
def get_canteenID_by_manager_account():
     account = request.form['account']
     cur = conn.cursor()
     cur.execute("select canteenID from managerInfo, manager_canteen where managerInfo.account={} and manager_canteen.managerID=managerInfo.managerID".format(account))
     canteenID=cur.fetchone()
     print(account)
     print(canteenID)
     data = {}
     # 非空
     if(canteenID):
          data={'code':1, 'canteenID': canteenID[0]}
     else:
          data={'code': 0}
     cur.close()
     return data

@app.route("/getStalls", methods=["POST"])
def get_stalls_by_manager_account():
     account = request.form['account']
     cur = conn.cursor()
     cur.execute("select canteenID from managerInfo, manager_canteen where managerInfo.account={} and manager_canteen.managerID=managerInfo.managerID".format(account))
     canteenID=cur.fetchone()
     data={}
     if(canteenID!=''):
          canteenID = canteenID[0]
          cur.execute("select stallKeeperInfo.name, stallKeeperInfo.stallKeeperID, stall.stallName, stall.stallImageUrl\
                         from stall, stallKeeperInfo\
                         where stall.canteenID={} and stallKeeperInfo.stallKeeperID=stall.stallKeeperID".format(canteenID))
          stalls=cur.fetchall()
          data={'stalls': stalls}
     else:
          data={'code': -1}
     cur.close()
     print(data)
     return data

@app.route("/getCommentByID", methods=["POST"])
def get_comment_by_id():
     commentID = request.form['commentID']
     cur = conn.cursor()
     cur.execute("select dishRank, stallRank, canteenRank, comment from comment where commentID={}".format(commentID))
     res = cur.fetchone()
     data = {'dishRank': res[0], 'stallRank': res[1], 'canteenRank': res[2], 'remark': res[3]}
     return data


@app.route("/getCanteenStallsInfo", methods=["POST"])
def get_stalls_in_view():
     print(request)
     canteenID = request.form['canteenID']
     cur = conn.cursor()
     cur.execute("select stallName, sum(amount) from canteen_stall_view where canteenID={}".format(canteenID))
     res = cur.fetchall()
     return {'stalls': res}


# 用于检查array中是否存在num
def check_list(array, num):
     for item in array:
          if(item==num):
               return True
     return False








if __name__ == '__main__':
    server = pywsgi.WSGIServer(('0.0.0.0', 5000), app)
    server.serve_forever()