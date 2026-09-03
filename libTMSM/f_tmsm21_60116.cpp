/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-01 18:08:50
Description: 中包烘烤状态变更
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tmsm21_60116(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString pono = " ";
	CString htno = " "; //熔炼号
	CString l_cs_no1 = " ";
	CString l_cs_no2 = " ";
	CDecimal l_tundish_total_age1 = 0;
	CDecimal l_tundish_total_age2 = 0;
	CDecimal v_count = 0;
	CDecimal coil_usage = 0;

	/* 实体类定义 */
	//CModel ttmsm31("TTMSM31");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_temp;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		htno = bcls_rec->Tables[0].Rows[0]["HTNO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "需要更新状态的熔炼号 = [{0}]", pono);

		//select TURRET_ARM into l_TURRET_ARM	TURRET_ARM 钢包臂（A，B）
		//	from LT_CC_FACT  连铸实绩
		//where htno = p_htno;

		//select coil_usage + 1, coil_factory, seat_usage + 1, seat_factory into l_coil_usage, l_coil_factory, l_seat_usage, l_seat_factory
		//	from LE_COIL_USAGE
		//where create_time = (select max(create_time) from LE_COIL_USAGE where LADLE_NO = l_LADLE_NO);
		//以上原一炼钢因为表和字段与产品化不同，按照先搭架子，后面根据现场要求再调整
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT USE_SEQ_NO+1 FROM TTMSM21 WHERE REC_CREATE_TIME = (SELECT MAX(REC_CREATE_TIME) FROM TTMSM21 )";
			break;
		}
		cmd_inq.Parameters.Clear();
		//cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			//l_cs_no1 = cmd_inq.GetString(1);
			//l_cs_no2 = cmd_inq.GetString(2);
			coil_usage = cmd_inq.GetDecimal(1);
			//l_tundish_total_age2 = cmd_inq.GetDecimal(4);
		}
		else
		{
			//l_cs_no1 = " ";
			//l_cs_no2 = " ";
			coil_usage = 0;
			//l_tundish_total_age2 = 0;
			//strcpy(s.msg, "1流结晶器号不能为空!");
			//throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();


		sqlstr = " SELECT COUNT(1) FROM TTMSM21"
				 " WHERE HEAT_NO =" + htno +  "'";
		v_count = Db::QueryCDecimal(sqlstr);
		if (v_count == 0)
		{

			//以下原来一炼钢
	/*		insert into LE_COIL_USAGE(LADLE_NO, LADLE_AGE, GYRE_ARMNO, CREATE_TIME, UPDATE_TIME, HTNO, CCM_NO, USAGE_NOTE, coil_usage, coil_factory, seat_usage, seat_factory)
			values(l_LADLE_NO, l_LADLE_AGE, l_TURRET_ARM, sysdate, sysdate, p_htno, l_ccm_no, '0', l_coil_usage, l_coil_factory, l_seat_usage, l_seat_factory);*/

			//中间包基本信息表
			//20230601没明白为啥要插入新纪录，也因为表和字段的原因，无法先从历史表拉初始数据
			//sqlstr = "INSERT　INTO ";
			//Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			//Db::Execute(sqlstr);
		}


		//ttmsm92["REC_CREATOR"] = s.userid;			//记录创建责任者
		//ttmsm92["REC_CREATE_TIME"] = datetime;		//记录创建时刻
		//ttmsm92["REC_REVISOR"] = s.userid;			//记录修改责任者
		//ttmsm92["REC_REVISE_TIME"] = datetime;		//记录修改时刻
		//抛其它工器具跟踪履历
		if (!bcls_rec->Tables.Contains("TMSM67"))
		{
			bcls_rec->Tables.Add("TMSM67");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "SM_UNIT_NO");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "REMARK_1");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "REMARK");
		}
		bcls_rec->Tables["TMSM67"].Rows.Clear();

		//其它工器具跟踪履历跟踪函数
		bcls_rec->Tables["TMSM67"].Rows.Add();
		bcls_rec->Tables["TMSM67"].Rows[0]["SM_UNIT_NO"] = "Z";				//用67表记录其它工器具履历
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "２０２３０６０１啥也没干";  //时间操作
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK"] = sqlstr;				 //事件
		doFlag = f_tmsm67_trace(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
	return doFlag;
}


