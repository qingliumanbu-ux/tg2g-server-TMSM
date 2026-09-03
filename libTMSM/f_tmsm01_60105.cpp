/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-03 09:43:15
Description: 转炉生产事件信息出钢开始钢包使用开始
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tmsm01_60105(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;

	CString cast_st = " "; //浇注开始时刻
	CString htno = " "; //熔炼号
	CDecimal v_count = 0;
	CDecimal lf_time = 0;
	CString l_steel_grade = " "; //出钢记号
	CString l_ladle_no = " ";    //钢包号
	CString l_bof_no = " ";
	CString l_maint_type = " ";
	CDecimal l_cast_div_no = 0;

	CString l_ld_type = " ";
	CString l_ld_status = " ";                                          
	CDecimal l_ladle_age = 0;
	CString l_bottom_blow_ins = " ";
	//CString l_up_temp_request = " ";
	CString l_drying_et = " ";
	CDecimal l_cross_lf_count = 0;
	CDecimal l_cross_lf_heat_time = 0;
	CDecimal l_count_fill_time = 0;
	CDecimal l_ladle_tare_weight = 0;

	CString l_shiftteam = " "; //                     --班组
	CString l_recorder = " "; //                      --责任人
	CString l_up_temp_request = " "; //               --提温要求
	CString l_up_temp_reason = " "; //                --提温原因

	CString l_yls_factory = " "; //生产厂家代码

	/* 实体类定义 */
	CModel ttmsm97("TTMSM97");

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

		Log::Trace("", __FUNCTION__, "需要更新状态的熔炼号 = [{0}]", htno);

	//	FOR rData IN crGet_lo_tap_plan LOOP                                          --取熔炼号对应的钢包号, 出钢记号, 转炉号
	//	l_ladle_no : = rData.ladle_no;
	//	l_steel_grade: = rData.steel_grade;
	//	l_bof_no: = rData.bof_no;
	//	END LOOP;
		//                                                     对应 TPSSM11
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			//炼钢计划运行监控表
			sqlstr = "SELECT MAX(LADLE_NO), " //出钢记号   
				"MAX(ST_NO), "//出钢记号 
				"' ' "		//BOF_NO 转炉号20230603没这个字段，暂时赋值为空
				"FROM TPSSM11 WHERE HEAT_NO = @htno "; //炼钢作业计划编制主表
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_ladle_no		= cmd_inq.GetString(1); // --钢包号
			l_steel_grade	= cmd_inq.GetString(2);
			l_bof_no		= cmd_inq.GetString(3);

		}
		cmd_inq.Close();

		//if l_ladle_no is null then
		//	RAISE_APPLICATION_ERROR(-20001, '该炉号没有排钢包计划!');
		//end if;
		if (l_ladle_no.Trim().GetLength()  == 0)
		{
			Log::Trace("", "", "该炉号没有排钢包计划：{0}", l_ladle_no);
			return doFlag;
		}

		//update  le_ld_status_trace_m                                                    --修改钢包跟踪状态
		//	set  htno = p_htno,
		//	steel_grade = l_steel_grade
		//where  ladle_no = l_ladle_no;
		sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
			"REC_REVISE_TIME = '" + datetime + "',"
			"ST_NO = '" + l_steel_grade + "',"
			"HEAT_NO = '" + htno + "',"
			" WHERE LADLE_NO = '" + l_ladle_no + "'";

		Log::Trace("", "", "sqlstr：{0}", sqlstr);
		Db::Execute(sqlstr);

		//------ - 维修类型为中修时，盛钢时间累计清零------------------------------------------------------
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT MAX(MAINT_TYPE) " //维修类型    epep03 tms3
				"FROM TTMSM01  WHERE LADLE_NO = '" + l_ladle_no + "'"; 
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_maint_type = cmd_inq.GetString(1); 
		}
		cmd_inq.Close();

		if (l_maint_type == "M")
		{
			//update le_ld_status_trace_m                                                    --修改钢包跟踪状态
			//	set  count_fill_time = 0
			//where  ladle_no = l_ladle_no;
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"COUNT_FILL_TIME = 0" //盛钢时间累计
				" WHERE LADLE_NO = '" + l_ladle_no + "'";

			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//以下逻辑未找到产品化的表字段
		//update le_ld_status_trace_d
		//	set ACT_WORK_CHARGE = nvl(ACT_WORK_CHARGE, 0) + 1  ACT_WORK_CHARGE    ///  实际工作炉数
		//where ladle_no = l_ladle_no;
		//commit;
		//for rData In crGet_le_ld_status_trace_d(l_ladle_no) loop
		//if rData.CHILD_EQU_NO = '01' then
		//l_UPWATER_DEGREE : = nvl(rData.ACT_WORK_CHARGE, 0);
		//elsif rData.CHILD_EQU_NO = '02' then
		//l_DOWNWATER_DEGREE : = nvl(rData.ACT_WORK_CHARGE, 0);
		//elsif rData.CHILD_EQU_NO = '05' then
		//l_SEAT_BRICK_DEGREEZ : = nvl(rData.ACT_WORK_CHARGE, 0);
		//elsif rData.CHILD_EQU_NO = '06' then
		//l_DREGS_LINE_DEGREE : = nvl(rData.ACT_WORK_CHARGE, 0);
		//elsif rData.CHILD_EQU_NO = '04' then
		//l_TQS_DEGREE : = nvl(rData.ACT_WORK_CHARGE, 0);
		//elsif rData.CHILD_EQU_NO = '03' then
		//l_LIANH_FRE : = nvl(rData.ACT_WORK_CHARGE, 0);
		//end if;
		//end loop;

	//l_counter: = Le_65001('ld');                                              --上次最大序号
	//l_counter_next : = Le_65001('ld') + 1;                                           --本次序号

	// BX_60006(l_systime, l_shift_no_1, l_shift_group_1);
		//	FOR rData IN crGet_le_ld_maint_his_m(l_ladle_no) LOOP
		//	l_recorder : = rData.recorder;                                               --取钢包号对应的责任人
		//	l_shiftteam : = rData.shiftteam;
		//	l_up_temp_reason: = rData.up_temp_reason;
		//	END LOOP;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT LD_TYPE,"//    钢包类型
				"LADLE_STATUS, "//  钢包状态
				"LADLE_LIFE, "//  包龄
				"' ',"//l_bottom_blow_ins  底吹情况
				"' ',"//l_up_temp_request  提温要求
				"DRYING_ET, "// 烘烤结束时刻
				"CROSS_LF_COUNT, "//  过LF炉次数累计
				"CROSS_LF_HEAT_TIME, "// LF炉加热时间累计
				"COUNT_FILL_TIME,"//   盛钢时间累计
				"EMPTY_LADLE_WT,   "//空包重量
				"REPAIR_WRITER,"	//   记录人
				"REPAIR_GRP_NO,,"
				"' ', "				/////提温原因
				"MANUFAC_NAME"		//厂家名称
				"FROM TTMSM01  WHERE LADLE_NO = '" + l_ladle_no + "'";
			break;
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("htno", htno);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_ld_type = cmd_inq.GetString(1);
			l_ld_status = cmd_inq.GetString(2);
			l_ladle_age = cmd_inq.GetDecimal(3);
			l_bottom_blow_ins = cmd_inq.GetString(4);
			l_up_temp_request = cmd_inq.GetString(5);
			l_drying_et = cmd_inq.GetString(6);
			l_cross_lf_count = cmd_inq.GetDecimal(7);
			l_cross_lf_heat_time = cmd_inq.GetDecimal(8);
			l_count_fill_time = cmd_inq.GetDecimal(9);
			l_ladle_tare_weight = cmd_inq.GetDecimal(10);
			l_recorder = cmd_inq.GetString(11);
			l_shiftteam = cmd_inq.GetString(12);
			l_up_temp_reason = cmd_inq.GetString(13); 
			l_yls_factory = cmd_inq.GetString(14);
	
		}
		cmd_inq.Close();

		ttmsm97["REC_CREATE_TIME"]  = datetime; //'记录创建时刻
		ttmsm97["REC_CREATOR"]		= s.userid; //记录创建责任者
		ttmsm97["REC_REVISE_TIME"]	= datetime; // 记录修改时刻
		ttmsm97["REC_REVISOR"]		= s.userid; //记录修改责任者
		ttmsm97["REC_ERASE_TIME"]	= " ";		//记录删除时间',
		ttmsm97["REC_ERASOR"]		= " ";		 //记录删除责任者',
		ttmsm97["ARCHIVE_FLAG"]		= " ";		//'归档标记',
		ttmsm97["EVENT_ID"]			= " ";		//事件标识',
		ttmsm97["EVENT_DESC"]		= "转炉生产事件信息出钢开始"; //'事件说明',
		ttmsm97["RESUME_SEQ_NO"] = atoi(EPGetNextSeq("TMSME_95", conn));
		ttmsm97["FUNC_ID"] = "f_tmsm01_60105" ;//功能标识',
		ttmsm97["FORM_NAME"] = "";//画面名称',
		ttmsm97["SM_UNIT_NO"] = "A";//'炼钢单元号',
		ttmsm97["STATUS"] = l_ld_status; //状态',
		ttmsm97["BAKE_POS"] = ""; //烘烤位置',
		ttmsm97["DRYING_ST"] = "";//"烘烤开始时刻',
		ttmsm97["DRYING_ET"] = l_drying_et; // '烘烤结束时刻',
		ttmsm97["DRYING_REMARK"] = " "; // 烘烤备注',
		//ttmsm97["REPAIR_SEQ_NO" IS '维修序号',
			//ttmsm97["MAINT_TYPE" IS '维修类型',
			//ttmsm97["REPAIR_POS" IS '维修位置',
			//ttmsm97["REPAIR_START_TIME" IS '维修开始时刻',
			//ttmsm97["REPAIR_END_TIME" IS '维修结束时刻',
			//ttmsm97["MAINTAIN_REMARK" IS '维修备注',
			//ttmsm97["SCRAP_TIME" IS '报废时刻',
			//ttmsm97["SCRAP_MAKER" IS '报废责任者',
			//ttmsm97["SCRAP_CAUSE_CODE" IS '报废原因代码',
			//ttmsm97["SCRAP_CAUSE_NAME" IS '报废原因名称',
			//ttmsm97["USE_SEQ_NO" IS '使用流水号',
			//ttmsm97["CC_MACH_NO" IS '连铸机号',
			//ttmsm97["STRAND_NO" IS '流号',
			ttmsm97["LADLE_LIFE"] = l_ladle_age; // '包龄',
			//ttmsm97["TD_COVER_NO" IS '中间包包盖号',
			//ttmsm97["PROD_DATETIME" IS '生产时刻_时分秒',
			//ttmsm97["START_USE_DATE" IS '开始启用日期',
			//ttmsm97["ONLINE_GROUP" IS '上包班别',
			//ttmsm97["ONLINE_TIME" IS '挂牌时间',
			//ttmsm97["ON_LINE_TIME" IS '上线时间',
			//ttmsm97["OFF_LINE_TIME" IS '下线时间',
			//ttmsm97["UNLADE_REASON" IS '下线原因',
			//ttmsm97["PONO" IS '制造命令号',
		ttmsm97["HEAT_NO"] = htno;				//熔炼号',
			//ttmsm97["CAST_START_GROUP" IS '浇铸开始班别',
			//ttmsm97["CAST_START_TIME" IS '浇铸开始时刻',
			//ttmsm97["CAST_END_TIME" IS '浇铸结束时刻',
			//ttmsm97["CAST_STOP_REASON" IS '停浇原因',
			//ttmsm97["INSPECT_MAKER" IS '检验人员',
			//ttmsm97["DEV_REMARK_1" IS '设备备注1',
			//ttmsm97["USE_REMARK" IS '使用备注',
			//ttmsm97["MOLD_SECTION" IS '结晶器断面',
			//ttmsm97["REMARK" IS '备注',
			//ttmsm97["COPPER_CHANGE_DIV" IS '更换铜管板标记',
			//ttmsm97["MD_COPPER_PLATE_NO" IS '铜管板号',
			//ttmsm97["COPPER_TIMES" IS '铜板/管使用次数',
			//ttmsm97["MOLD_NUM" IS '结晶器使用炉数',
			//ttmsm97["TOTAL_CHARGE_NUM" IS '累计总炉数',
			//ttmsm97["STD_CHARGE_NUM" IS '标准使用炉数',
			//ttmsm97["CUR_CAST_LEN" IS '当前浇铸长度',
			//ttmsm97["TOTAL_CAST_WT" IS '总浇铸重量',
			//ttmsm97["TOTAL_CAST_LEN" IS '总浇铸长度',
			//ttmsm97["COPPER_TIMES_1" IS '铜板1使用次数',
			//ttmsm97["COPPER_TIMES_2" IS '铜板2使用次数',
			//ttmsm97["COPPER_TIMES_3" IS '铜板3使用次数',
			//ttmsm97["COPPER_TIMES_4" IS '铜板4使用次数',
			//ttmsm97["BACK_N1" IS '备用字段N1',
			//ttmsm97["BACK_N2" IS '备用字段N2',
			//ttmsm97["BACK_N3" IS '备用字段N3',
			//ttmsm97["BACK_N4" IS '备用字段N4',
			//ttmsm97["BACK_N5" IS '备用字段N5',
			//ttmsm97["SPARE_ITEM_N3" IS '备用字段_N3',
			//ttmsm97["SPARE_ITEM_N4" IS '备用字段_N4',
			//ttmsm97["BACK_C1" IS '备用字段C1',
			//ttmsm97["BACK_C2" IS '备用字段C2',
			//ttmsm97["BACK_C3" IS '备用字段C3',
			//ttmsm97["BACK_C4" IS '备用字段C4',
			//ttmsm97["BACK_C5" IS '备用字段C5',
			//ttmsm97["BACK_C6" IS '备用字段C6',
			//ttmsm97["BACK_C7" IS '备用字段C7',
			//ttmsm97["BACK_C8" IS '备用字段C8',
			//ttmsm97["BACK_C9" IS '备用字段C9',
			//ttmsm97["MOLD_NO" IS '结晶器号',
			//ttmsm97["MOLD_TYPE" IS '设备类型2',
			//ttmsm97["BASE_CODE" IS '基地代码',
			//ttmsm97["BASE_NAME" IS '基地名称');

		ttmsm97.TrimOrBlank();
		ttmsm97.Insert();

		if (l_ld_status == "2")                               // --热修或热备烘烤目前TMS3是2，热修
			// update  le_ld_status_trace_m
			// set  ld_status = '4', --使用状态
			// usage_st = l_systime,
			// usage_et = null, --将上次使用结束时间置空
			// drying_et = nvl(drying_et, l_systime),
			// maint_et = nvl(maint_et, l_systime),
			// ladle_age = nvl(ladle_age, 0) + 1, --累计钢包龄
			// htno = p_htno,
			// steel_grade = l_steel_grade
			//where  ladle_no = l_ladle_no;
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"LADLE_LIFE = LADLE_LIFE + 1," //包龄
				"LADLE_STATUS = '4'," //钢包状态  20230603TMS3还没有4的状态，一炼钢代码设备管理中钢包状态4为使用中
				"START_USE_DATE = ' '," //开始启用日期
				"DRYING_ET = '" + datetime + "'," //使用开始时间
				"REPAIR_END_TIME= '" + datetime + "'," //维修结束时刻
				"USAGE_ST = '" + datetime + "'," //使用开始时间
				"USAGE_ET =  ' ' " //使用结束时间
				" WHERE LADLE_NO = '" + l_ladle_no + "'";


		}
		else
		{
			sqlstr = "UPDATE TTMSM01 SET REC_REVISOR= '" + userid + "',"
				"LADLE_LIFE = LADLE_LIFE + 1," //包龄
				"LADLE_STATUS = '4'," //钢包状态   20230603TMS3还没有4的状态，一炼钢代码设备管理中钢包状态4为使用中
				"START_USE_DATE = ' '," //开始启用日期
				"USAGE_ST = '" + datetime + "'," //使用开始时间
				"USAGE_ET =  ' '," //使用结束时间
				"REMARK =  '使用中' " //使用结束时间
				" WHERE LADLE_NO = '" + l_ladle_no + "'";


		}
		Log::Trace("", "", "sqlstr：{0}", sqlstr);
		Db::Execute(sqlstr);
	

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
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "转炉生产事件信息出钢开始钢包使用开始";  //时间操作
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


