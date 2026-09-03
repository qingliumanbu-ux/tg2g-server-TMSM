/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-17 18:37:40
Description: 工器具基本信息删除
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme91a1_del)


int f_tmsme91a1_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm91("TTMSM91");
	CModel ttmsm92 = CModel("TTMSM92");
	CModel ttmsm95("TTMSM95");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
			Log::Trace("", __FUNCTION__, "datetime		= [{0}]", datetime);
			/* ***** 获取输入参数 ***** */
			//if (bcls_rec->Tables["TMSME61_INQS"].Columns.Contains("DEV_TYPE"))
			if (bcls_rec->Tables[0].Columns.Contains("SM_UNIT_NO"))
				ttmsm91["SM_UNIT_NO"] = bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"];
			if (bcls_rec->Tables[0].Columns.Contains("DEV_NO"))
				ttmsm91["DEV_NO"] = bcls_rec->Tables[0].Rows[0]["DEV_NO"];

			Log::Trace("", __FUNCTION__, "ttmsm91.SM_UNIT_NO		= [{0}]", (const char*)ttmsm91["SM_UNIT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm91.DEV_NO		= [{0}]", (const char*)ttmsm91["DEV_NO"].ToString());

			/* 检查输入参数合法性 */
			if (ttmsm91["DEV_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "设备编号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		
			/* 删除工器具基本信息表信息 */
			ttmsm91.Delete("SM_UNIT_NO,DEV_NO");

			/* 删除工器具基本信息表信息 */
			ttmsm92.Delete("SM_UNIT_NO,DEV_NO");


			sqlstr = "UPDATE TTMSM91 SET MAIN_DEV_NO= ' ',"
				" SECTION_MODE = ' ',SECTION_DEFINE = ' ' "
				" WHERE SM_UNIT_NO = '" + ttmsm91["SM_UNIT_NO"].ToString() + "' AND MAIN_DEV_NO = '" + ttmsm91["DEV_NO"].ToString() + "'";
			Db::Execute(sqlstr);

			Log::Trace("", "", "sqlstr：{0}", sqlstr);

			//写95履历表
			ttmsm95["REC_CREATOR"] = s.userid;							// 记录创建责任者, ,
			ttmsm95["REC_CREATE_TIME"] = datetime;						//记录创建时刻
			ttmsm95["REC_REVISOR"] = s.userid;							//记录修改责任者
			ttmsm95["REC_REVISE_TIME"] = datetime;						//记录修改时刻
			ttmsm95["ARCHIVE_FLAG"] = " ";								//归档标记   
			ttmsm95["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));	//序号
			ttmsm95["SM_UNIT_NO"] = ttmsm91["SM_UNIT_NO"];				//炼钢单元号
			ttmsm95["WORK_AREA"] = " " ;		//作业区
			ttmsm95["DEV_TYPE"] = " ";					//设备类别
			ttmsm95["DEV_NAME"] = " ";					//设备名称
			ttmsm95["DEV_NO"] = ttmsm91["DEV_NO"];									//设备编号_001
			ttmsm95["STATUS"] = " ";									//状态
			ttmsm95["S_DATETIME"] = " ";								//开始时间
			ttmsm95["E_DATETIME"] = " ";								//结束时间
			ttmsm95["STATUS_DESC"] = " ";								//状态描述
			ttmsm95["MANU_CODE"] = " ";									//制造厂代码
			ttmsm95["MANUFAC_NAME"] = " ";								//厂家名称
			//ttmsm95["WT"] = ;											//重量
			//ttmsm95["PLAN_NUM"] = ;									//计划个数
			//ttmsm95["CUR_NUM"] = ;									//当前条数
			//ttmsm95["TOTAL_NUM"] = ;									//总件数（支数）
			//ttmsm95["REPAIR_NUM"] = ;									//返修次数				
			ttmsm95["LAST_STATUS"] = " ";								//上次状态代码
			ttmsm95["LAST_STATUS_START_TIME"] = " ";					//上次状态开始时刻
			ttmsm95["LAST_STATUS_END_TIME"] = " ";						//上次状态结束时刻
			//ttmsm95["LAST_SEQ_NO"] = ;			//上次L3顺序号
			ttmsm95["CHANGE_RES"] = " ";			//更换原因代码
			ttmsm95["CHANGE_RES_C"] = " ";			//更换原因描述
			ttmsm95["SCRAP_REASON_C"] = " ";		//判废原因描述
			ttmsm95["SCRAP_REASON_CODE"] = " ";		//判废原因代码
			ttmsm95["HEAT_NO"] = " ";				//熔炼号
			ttmsm95["SHIFT_GROUP_S"] = " ";			//四班开始班组
			ttmsm95["SHIFT_NO_S"] = " ";			//四班开始班次
			ttmsm95["GRP_NO"] = " ";				//班别号
			ttmsm95["SHIFT_NO"] = " ";				//班次号
			ttmsm95["RESP"] = " ";					//责任人
			ttmsm95["START_SHIFT_NO"] = " ";		//开始班次号
			ttmsm95["START_SHIFT_GROUP"] = " ";		//开始班组
			ttmsm95["END_SHIFT_NO"] = " ";			//结束班次号
			ttmsm95["END_SHIFT_GROUP"] = " ";		//结束班组
			ttmsm95["STAFF_ID"] = " ";				//工号
			ttmsm95["MAIN_DEV_NO"] = " ";			//主设备编号
			ttmsm95["SECTION_MODE"] = " ";			//分段方式代码
			ttmsm95["SECTION_DEFINE"] = " ";		//分段定义
			ttmsm95["OPERATE_TIME"] = " ";			//操作时刻
			ttmsm95["OPERATE_MODE"] = " ";			//操作模式
			ttmsm95["REMARK"] = " ";				//备注
			ttmsm95["INSPECT_CONCL"] = " ";			//检测结论
			//ttmsm95["CURRENT_FREQ"] = ;			//当前频次
			//ttmsm95["TOTAL_FREQ"] = ;				//累计频次
			//ttmsm95["BACK_N1"] = ;				//备用字段N1
			//ttmsm95["BACK_N2"] = ;				//备用字段N2
			//ttmsm95["BACK_N3"] = ;				//备用字段N3
			//ttmsm95["BACK_N4"] = ;				//备用字段N4
			//ttmsm95["BACK_N5"] = ;				//备用字段N5
			//ttmsm95["SPARE_ITEM_N3"] = ;			//备用字段_N3
			//ttmsm95["SPARE_ITEM_N4"] = ;			//备用字段_N4
			ttmsm95["BACK_C1"] = " ";		//备用字段C1
			ttmsm95["BACK_C2"] = " ";		//备用字段C2
			ttmsm95["BACK_C3"] = " ";		//备用字段C3
			ttmsm95["BACK_C4"] = " ";		//备用字段C4
			ttmsm95["BACK_C5"] = " ";		//备用字段C5
			ttmsm95["BACK_C6"] = s.fore_machine;		//备用字段C6
			ttmsm95["BACK_C7"] = s.fore_ip;				//备用字段C7
			ttmsm95["BACK_C8"] = "tmsme91a1_del";		//备用字段C8  用于记录后台服务
			ttmsm95["BACK_C9"] = "工器具基本信息删除";		//备用字段C9  用于事件标识
			ttmsm95["BASE_CODE"] = " ";		//基地代码
			ttmsm95["BASE_NAME"] = " ";		//基地名称
			ttmsm95.TrimOrBlank();
			ttmsm95.Insert();

		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


