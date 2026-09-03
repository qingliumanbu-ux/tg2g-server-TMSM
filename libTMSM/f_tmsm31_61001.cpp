/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-01 10:32:54
Description: 铜板电镀后使用累计数及总使用数计算
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tmsm31_61001(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString pono = " ";
	CString l_cs_no1 = " ";
	CString l_cs_no2 = " ";

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
		pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "需要更新状态的制造命令号 = [{0}]", pono);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "select MAX(MOLD_NO1),MAX(MOLD_NO2) FROM TMMSM31 WHERE PONO= @pono";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			l_cs_no1 = cmd_inq.GetString(1);
			l_cs_no2 = cmd_inq.GetString(2);
		}
		else
		{
			l_cs_no1 = " ";
			l_cs_no2 = " ";
			strcpy(s.msg, "1流结晶器号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();

		
		if ((l_cs_no1.Trim().GetLength() == 0) && (l_cs_no2.Trim().GetLength() == 0))
		{
			strcpy(s.msg, "没有可用结晶器!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (l_cs_no1.Trim().GetLength() > 0)
		{
			//20230601暂时从TTMSM31结晶器基本信息表获取，未从铜板基本信息表TTMSM41
			sqlstr = " SELECT * FROM TTMSM31 WHERE MOLD_NO = '" + l_cs_no1.Trim() + "'"
				" ORDER BY MD_COPPER_PLATE_NO";
			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::QueryTable(sqlstr, dt_temp);
			//Log::Trace("", "", "Count：{0}", dt_temp.Rows.get_Count());
			for (int i = 0; i < dt_temp.Rows.get_Count(); i++)
			{
				//IF rData.maint_type = '01' THEN 维修类型
				//不是CP_TYPE铜板类型
				//TTMSM41 TM_COPPER_TYPE 铜板类型
				if (dt_temp.Rows[i]["TM_COPPER_TYPE"].ToString().Trim() == "01")
				{
					//UPDATE le_cp_status_trace
					//	SET ddhgg_stove_degree = NVL(ddhgg_stove_degree, 0) + 1, 电镀后过钢炉数
					//	tblj_using_degree = NVL(tblj_using_degree, 0) + 1 铜板累计使用炉数
					//	WHERE copperplate_no = rData.copperplate_no;
					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1,"  //累计总炉数
						"DDHGG_CHARGE_NUM = DDHGG_CHARGE_NUM +1 "  //电镀后过钢炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER1"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 ,"//累计总炉数
						"DDHGG_CHARGE_NUM = DDHGG_CHARGE_NUM + 1 "  //电镀后过钢炉数"  
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER2"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1, "//累计总炉数
						"DDHGG_CHARGE_NUM = DDHGG_CHARGE_NUM + 1 "  //电镀后过钢炉数" 
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER3"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1, "//累计总炉数
						"DDHGG_CHARGE_NUM = DDHGG_CHARGE_NUM + 1 "  //电镀后过钢炉数" 
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER4"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

				}
				else
				{
					//UPDATE le_cp_status_trace
					//	SET tblj_using_degree = NVL(tblj_using_degree, 0) + 1 铜板累计使用炉数
					//	WHERE copperplate_no = rData.copperplate_no;
					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER1"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER2"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER3"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER4"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);
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
				bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "l_cs_no1更新钢包位置";  //时间操作
				bcls_rec->Tables["TMSM67"].Rows[0]["REMARK"] = sqlstr;				 //事件
				doFlag = f_tmsm67_trace(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}
		}

		dt_temp.Clear();
		if (l_cs_no2.Trim().GetLength() > 0)
		{
			//20230601暂时从TTMSM31结晶器基本信息表获取，未从铜板基本信息表TTMSM41
			sqlstr = " SELECT * FROM TTMSM31 WHERE MOLD_NO = '" + l_cs_no2.Trim() + "'"
				" ORDER BY MD_COPPER_PLATE_NO";
			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::QueryTable(sqlstr, dt_temp);
			//Log::Trace("", "", "Count：{0}", dt_temp.Rows.get_Count());
			for (int i = 0; i < dt_temp.Rows.get_Count(); i++)
			{
				//IF rData.maint_type = '01' THEN 维修类型
				//不是CP_TYPE铜板类型
				//TTMSM41 TM_COPPER_TYPE 铜板类型
				if (dt_temp.Rows[i]["TM_COPPER_TYPE"].ToString().Trim() == "01")
				{
					//UPDATE le_cp_status_trace
					//	SET ddhgg_stove_degree = NVL(ddhgg_stove_degree, 0) + 1, 电镀后过钢炉数
					//	tblj_using_degree = NVL(tblj_using_degree, 0) + 1 铜板累计使用炉数
					//	WHERE copperplate_no = rData.copperplate_no;
					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER1"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER2"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER3"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER4"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

				}
				else
				{
					//UPDATE le_cp_status_trace
					//	SET tblj_using_degree = NVL(tblj_using_degree, 0) + 1 铜板累计使用炉数
					//	WHERE copperplate_no = rData.copperplate_no;
					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER1"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						""                                       //电镀后过钢炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER2"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER3"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);

					sqlstr = "UPDATE TTMSM41 SET REC_REVISOR= '" + userid + "',"
						"REC_REVISE_TIME = '" + datetime + "',"
						"TOTAL_CHARGE_NUM= TOTAL_CHARGE_NUM + 1 "//累计总炉数
						" WHERE TM_COPPER_NO = '" + dt_temp.Rows[i]["COPPER4"].ToString().Trim() + "'";
					//铜板编号
					Log::Trace("", "", "sqlstr1：{0}", sqlstr);
					Db::Execute(sqlstr);
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
				bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "l_cs_no2更新钢包位置";  //时间操作
				bcls_rec->Tables["TMSM67"].Rows[0]["REMARK"] = sqlstr;				 //事件
				doFlag = f_tmsm67_trace(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}
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


