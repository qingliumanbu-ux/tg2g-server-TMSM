/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-12
功能: 钢包使用实绩修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm09.h"
#include "ttmsm01.h"

/*<remark>=========================================================
/// <summary>
/// 钢包使用实绩修改
/// <para>
/// 1.接收传入的钢包使用实绩信息；
/// 2.修改钢包使用实绩信息；
/// </para>
/// <para>数据库表：TTMSM09(钢包使用实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面使用实绩修改调用；TTMSM02画面F4(修改)调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的钢包使用实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm09_upd)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm09_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */
	CString v_group = "";
	CString v_shift = "";
	CString v_div = "";
	
	/* 实体类定义 */
	CTTMSM09 ttmsm09(conn);
	CTTMSM01 ttmsm01(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{			
		//记录总条数
		int rows = bcls_rec->Tables[0].Rows.get_Count();
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm09_upd","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM04")
		{
			//循环处理
			for (int i = 0; i < rows ; i++ )
			{
				// 取得单行传入信息 
				ttmsm09.Reset();
				ttmsm09.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				Log::Trace("","tmsm09_upd","ttmsm09.USE_SEQ_NO [{0}]"	,ttmsm09.USE_SEQ_NO);
				Log::Trace("","tmsm09_upd","ttmsm09.LADLE_NO [{0}]"		,(const char*)ttmsm09.LADLE_NO);
				Log::Trace("","tmsm09_upd","ttmsm09.SM_UNIT_NO [{0}]"	,(const char*)ttmsm09.SM_UNIT_NO);
				Log::Trace("","tmsm09_upd","ttmsm09.USAGE_ST [{0}]"		,(const char*)ttmsm09.USAGE_ST);
				Log::Trace("","tmsm09_upd","ttmsm09.USAGE_ET [{0}]"		,(const char*)ttmsm09.USAGE_ET);

				//班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm09.USAGE_ST,v_shift,v_group,conn );

				// 判传入的参数是否有错	
				if(ttmsm09.USE_SEQ_NO <= 0)
				{
					sprintf	(s.msg , "传入的使用流水号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
				}
				if(ttmsm09.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
				}
				if(ttmsm09.LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的钢包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
				}
				if(ttmsm09.USAGE_ST.Trim() == "")
				{
					sprintf	(s.msg , "传入的使用起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
				}
				if(ttmsm09.USAGE_ET.Trim() == "")
				{
					sprintf	(s.msg , "传入的使用结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
				}
				
				/*查询钢包使用实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM09 "
						 "  WHERE USE_SEQ_NO	= @ttmsm09.USE_SEQ_NO "
						 "  AND SM_UNIT_NO		= @ttmsm09.SM_UNIT_NO "
						 "  AND LADLE_NO		= @ttmsm09.LADLE_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm09.USE_SEQ_NO"	, ttmsm09.USE_SEQ_NO );
				cmd_inq.Parameters.Set( "ttmsm09.LADLE_NO"		, ttmsm09.LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm09.SM_UNIT_NO"	, ttmsm09.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				Log::Trace("","tmsm09_upd","count [{0}]",count );
				cmd_inq.Close();
				if	(count == 0)
				{
					sprintf	(s.msg ,"钢包使用实绩信息不存在");
					/*新增钢包使用实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
				}
				
				//设置修改者信息
				ttmsm09.REC_REVISOR = s.userid;
				ttmsm09.REC_REVISE_TIME = datetimeNow;

				if(ttmsm09.GRP_NO.Trim() == "")
				{
					ttmsm09.GRP_NO = v_group;
				}
				if(ttmsm09.SHIFT_NO.Trim() == "")
				{
					ttmsm09.SHIFT_NO = v_shift;
				}
				//根据传入信息更新钢包使用实绩表
				sqlstr = "UPDATE TTMSM09"; //用于捕获数据库操作异常情况
				ttmsm09.Update(	" REC_REVISOR "
								" ,REC_REVISE_TIME "
								" ,LADLE_LIFE "			//包龄
								" ,USAGE_ST "			// 使用开始时间
								" ,USAGE_ET "			// 使用结束时间
								" ,SHELL_FAY "			// 外壳厂家
								" ,SHELL_USE_TIMES "	// 外壳使用次数
								" ,HEAT_NO "			// 熔炼号
								" ,PROD_DATE "			// 生产日期
								" ,SM_PLAN_NO "			// 炼钢计划号
								" ,TPS_NO "				// 出钢计划号
								" ,ST_NO "				// 出钢记号
								" ,ACT_REFINE_ROUTE "	// 实际精炼区分
								" ,PONO "				// 制造命令号
								" ,LADLE_W_L "			// 钢水重量
								" ,LOAD_STEEL_WT "		// 装钢量
								" ,LOAD_STEEL_WT_TOTAL "// 装钢重量累计
								" ,SEN_TIMES_UPPER "	// 上水口使用次数
								" ,NOZZLE_BRICK_MANU "	// 水口座砖厂家
								" ,NOZZLE_BRICK_TIMES "	// 水口座砖使用次数
								" ,LADLE_BRICK_TIMES "	// 钢包透气砖使用次数
								" ,BREATH_FAY "			// 透气砖厂家
								" ,BOF_NO "				// 转炉号
								" ,NOZZLE_SWITCH_TIMES "// 水口开关使用次数
								" ,SRP_TIME_TOTAL "		// 精炼时间累计
								" ,AR_BLOW_TIME_TOTAL "	// 吹氩时间累计
								" ,RH_TIME_TOTAL "		// RH时间累计
								" ,SLIDE_MAKER "		// 滑板厂家
								" ,SLIP_BOARD_USE_TIMES "	// 滑板使用次数
								" ,SLAGLINE_TIMES "		// 渣线次数
								" ,WORK_MAKER "			// 工作层(耐材厂家)
								" ,LADLE_CURRENT_AREA "	// 钢包当前位置
								" ,LADLE_ARRIVE_TIME "	// 钢包到达时刻
								" ,GRP_NO "				// 班别号
								" ,SHIFT_NO "			// 班次号		
								" ,USE_SEQ_NO,SM_UNIT_NO,LADLE_NO ");
			}
		}
		else if(v_div.Trim() == "TMSM01")
		{
			/*重置TTMSM05*/
			ttmsm01.Reset();
			/*读取传入的参数*/
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm09_upd","ttmsm01.LADLE_NO [{0}]"		,(const char*)ttmsm01.LADLE_NO);
			Log::Trace("","tmsm09_upd","ttmsm01.SM_UNIT_NO [{0}]"	,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace("","tmsm09_upd","ttmsm01.USAGE_ST [{0}]"		,(const char*)ttmsm01.USAGE_ST);
			Log::Trace("","tmsm09_upd","ttmsm01.USAGE_ET [{0}]"		,(const char*)ttmsm01.USAGE_ET);

			// 判传入的参数是否有错			
			if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			if(ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			if(ttmsm01.USAGE_ST.Trim() == "")
			{
				sprintf	(s.msg , "传入的使用起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			if(ttmsm01.USAGE_ET.Trim() == "")
			{
				sprintf	(s.msg , "传入的使用结束时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}

			//班次班组 shift_no shift_group
			f_epep_get_shift_group("SM",ttmsm01.USAGE_ST,v_shift,v_group,conn );

			/*查询钢包使用实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM09 "
					 "  WHERE SM_UNIT_NO	= @ttmsm09.SM_UNIT_NO "					 
					 "  AND USE_SEQ_NO		= @ttmsm09.USE_SEQ_NO "
					 "  AND LADLE_NO		= @ttmsm09.LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm09.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm09.USE_SEQ_NO"	, ttmsm01.USE_SEQ_NO );
			cmd_inq.Parameters.Set( "ttmsm09.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm09_upd","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count == 0)
			{
				sprintf	(s.msg ,"钢包使用实绩信息不存在");
				/*新增钢包使用实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm09_upd");
			}
			
			/*修改钢包使用实绩表*/
			ttmsm09.CopyFrom(ttmsm01);
			//ttmsm09.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			//ttmsm09.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm09.REC_REVISOR = s.userid;
			ttmsm09.REC_REVISE_TIME = datetimeNow;
			//班次班组 shift_no shift_group

			//s.datetime是前台终端操作系统的时间，而不是服务器时间
			if(ttmsm09.GRP_NO.Trim() == "")
			{
				ttmsm09.GRP_NO = v_group;
			}
			if(ttmsm09.SHIFT_NO.Trim() == "")
			{
				ttmsm09.SHIFT_NO = v_shift;
			}
			ttmsm09.TrimOrBlank();
			sqlstr = "UPDATE TTMSM09"; //用于捕获数据库操作异常情况
			ttmsm09.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,LADLE_LIFE "			//包龄
							" ,USAGE_ST "			//使用开始时间
							" ,USAGE_ET "			//使用结束时间
							" ,SHELL_FAY "			//外壳厂家
							" ,SHELL_USE_TIMES "	//外壳使用次数
							" ,HEAT_NO "			//熔炼号
							" ,PROD_DATE "			//生产日期
							" ,SM_PLAN_NO "			//炼钢计划号
							" ,TPS_NO "				//出钢计划号
							" ,ST_NO "				//出钢记号
							" ,ACT_REFINE_ROUTE "	//实际精炼区分
							" ,PONO "				//制造命令号
							" ,LADLE_W_L "			//钢水重量
							" ,LOAD_STEEL_WT "		//装钢量
							" ,LOAD_STEEL_WT_TOTAL "//装钢重量累计
							" ,SEN_TIMES_UPPER "	//上水口使用次数
							" ,NOZZLE_BRICK_MANU "  //水口座砖厂家
							" ,NOZZLE_BRICK_TIMES " //水口座砖使用次数
							" ,LADLE_BRICK_TIMES "  //钢包透气砖使用次数
							" ,BREATH_FAY "			//透气砖厂家
							" ,BOF_NO "				//转炉号
							" ,NOZZLE_SWITCH_TIMES "//水口开关使用次数
							" ,SRP_TIME_TOTAL "		//精炼时间累计
							" ,AR_BLOW_TIME_TOTAL " //吹氩时间累计
							" ,RH_TIME_TOTAL "		//RH时间累计
							" ,SLIDE_MAKER "		//滑板厂家
							" ,SLIP_BOARD_USE_TIMES "	//滑板使用次数
							" ,SLAGLINE_TIMES "		//渣线次数
							" ,WORK_MAKER "			//工作层(耐材厂家)
							" ,LADLE_CURRENT_AREA " //钢包当前位置
							" ,LADLE_ARRIVE_TIME "  //钢包到达时刻
							" ,GRP_NO "				//班别号
							" ,SHIFT_NO "			//班次号
							" ,USE_REMARK ",		//使用备注
							" USE_SEQ_NO,SM_UNIT_NO,LADLE_NO ");

			sqlstr = "UPDATE TTMSM01";
			ttmsm01.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,LADLE_LIFE "			//包龄
							" ,USAGE_ST "			//使用开始时间
							" ,USAGE_ET "			//使用结束时间
							" ,SHELL_FAY "			//外壳厂家
							" ,SHELL_USE_TIMES "	//外壳使用次数
							" ,HEAT_NO "			//熔炼号
							" ,PROD_DATE "			//生产日期
							" ,SM_PLAN_NO "			//炼钢计划号
							" ,TPS_NO "				//出钢计划号
							" ,ST_NO "				//出钢记号
							" ,ACT_REFINE_ROUTE "   //实际精炼区分
							" ,PONO "				//制造命令号
							" ,LADLE_W_L "			//钢水重量
							" ,LOAD_STEEL_WT "		//装钢量
							" ,LOAD_STEEL_WT_TOTAL "//装钢重量累计
							" ,SEN_TIMES_UPPER "	//上水口使用次数
							" ,NOZZLE_BRICK_MANU "  //水口座砖厂家
							" ,NOZZLE_BRICK_TIMES " //水口座砖使用次数
							" ,LADLE_BRICK_TIMES "  //钢包透气砖使用次数
							" ,BREATH_FAY "			//透气砖厂家
							" ,BOF_NO "				//转炉号
							" ,NOZZLE_SWITCH_TIMES "//水口开关使用次数
							" ,SRP_TIME_TOTAL "		//精炼时间累计
							" ,AR_BLOW_TIME_TOTAL " //吹氩时间累计
							" ,RH_TIME_TOTAL "		//RH时间累计
							" ,SLIDE_MAKER "		//滑板厂家
							" ,SLIP_BOARD_USE_TIMES "//滑板使用次数
							" ,SLAGLINE_TIMES "		//渣线次数
							" ,WORK_MAKER "			//工作层(耐材厂家)
							" ,LADLE_CURRENT_AREA " //钢包当前位置
							" ,LADLE_ARRIVE_TIME "  //钢包到达时刻
							" ,GRP_NO "				//班别号
							" ,SHIFT_NO "			//班次号
							" ,USE_REMARK  ",		//使用备注
							" SM_UNIT_NO,LADLE_NO ");			
		}
		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}