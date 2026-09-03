/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 
日期: 2014-05-07
功能: 钢包热修实绩修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm04.h"
#include "ttmsm01.h"

/*<remark>=========================================================
/// <summary>
/// 钢包热修实绩修改
/// <para>
/// 1.接收传入的钢包热修实绩信息；
/// 2.修改钢包热修实绩信息；
/// </para>
/// <para>数据库表：TTMSM04(钢包热修实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面使用实绩修改调用；TTMSM06画面F4(修改)调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的钢包热修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm06_upd)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm06_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM04 ttmsm04(conn);
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
		Log::Trace("","tmsm06_upd","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM04")
		{
			//循环处理
			for (int i = 0; i < rows ; i++ )
			{
				// 取得单行传入信息 
				ttmsm04.Reset();
				ttmsm04.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				Log::Trace("","tmsm06_upd","ttmsm04.USE_SEQ_NO [{0}]"	,ttmsm04.USE_SEQ_NO);
				Log::Trace("","tmsm06_upd","ttmsm04.LADLE_NO [{0}]"		,(const char*)ttmsm04.LADLE_NO);
				Log::Trace("","tmsm06_upd","ttmsm04.SM_UNIT_NO [{0}]"	,(const char*)ttmsm04.SM_UNIT_NO);
				Log::Trace("","tmsm06_upd","ttmsm04.REPAIR_START_TIME [{0}]"		,(const char*)ttmsm04.REPAIR_START_TIME);
				Log::Trace("","tmsm06_upd","ttmsm04.REPAIR_END_TIME [{0}]"		,(const char*)ttmsm04.REPAIR_END_TIME);

				//班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm04.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错	
				
				if(ttmsm04.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
				}
				if(ttmsm04.LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的钢包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
				}
				if(ttmsm04.REPAIR_START_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修开始时刻不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
				}
				if(ttmsm04.REPAIR_END_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修结束时刻不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
				}

				if(ttmsm04.HOT_REPAIR_DATE.Trim() != "")
				{
					ttmsm04.HOT_REPAIR_DATE = ttmsm04.HOT_REPAIR_DATE.Substring(0,8);
				}
				
				/*查询钢包使用实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM04 "
						 "  WHERE LADLE_NO		= @ttmsm04.LADLE_NO ";
						 
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm04.LADLE_NO"		, ttmsm04.LADLE_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				Log::Trace("","tmsm06_upd","count [{0}]",count );
				cmd_inq.Close();
				if	(count == 0)
				{
					sprintf	(s.msg ,"钢包使用实绩信息不存在");
					/*新增钢包使用实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
				}
				
				//设置修改者信息
				ttmsm04.REC_REVISOR = s.userid;
				ttmsm04.REC_REVISE_TIME = datetimeNow;

				if(ttmsm04.GRP_NO.Trim() == "")
				{
					ttmsm04.GRP_NO = v_group;
				}
				if(ttmsm04.SHIFT_NO.Trim() == "")
				{
					ttmsm04.SHIFT_NO = v_shift;
				}

				Log::Trace("","tmsm06_upd","ttmsm04.USE_LIFE2 [{0}]"		,ttmsm04.USE_LIFE2);
				Log::Trace("","tmsm06_upd","ttmsm04.RESIDUAL_THICK2 [{0}]"		,ttmsm04.RESIDUAL_THICK2);
				Log::Trace("","tmsm06_upd","ttmsm04.USE_LIFE3 [{0}]"		,ttmsm04.USE_LIFE3);
				Log::Trace("","tmsm06_upd","ttmsm04.RESIDUAL_THICK3 [{0}]"		,ttmsm04.RESIDUAL_THICK3);
				Log::Trace("","tmsm06_upd","ttmsm04.USE_LIFE4 [{0}]"		,ttmsm04.USE_LIFE4);
				Log::Trace("","tmsm06_upd","ttmsm04.RESIDUAL_THICK4 [{0}]"		,ttmsm04.RESIDUAL_THICK4);
				Log::Trace("","tmsm06_upd","ttmsm04.USE_LIFE5 [{0}]"		,ttmsm04.USE_LIFE5);
				Log::Trace("","tmsm06_upd","ttmsm04.RESIDUAL_THICK5 [{0}]"		,ttmsm04.RESIDUAL_THICK5);
				Log::Trace("","tmsm06_upd","ttmsm04.PACK_ALONG_HEIGHT [{0}]"		,ttmsm04.PACK_ALONG_HEIGHT);
				Log::Trace("","tmsm06_upd","ttmsm04.SAND_WT [{0}]"		,ttmsm04.SAND_WT);
				Log::Trace("","tmsm06_upd","ttmsm04.SHELL_UP_TEMP [{0}]"		,ttmsm04.SHELL_UP_TEMP);
				Log::Trace("","tmsm06_upd","ttmsm04.SHELL_DOWN_TEMP [{0}]"		,ttmsm04.SHELL_DOWN_TEMP);
				Log::Trace("","tmsm06_upd","ttmsm04.RESIDUAL_STEEL_THICK [{0}]"		,ttmsm04.RESIDUAL_STEEL_THICK);
				Log::Trace("","tmsm06_upd","ttmsm04.AFFIRM_FLAG [{0}]"		,ttmsm04.AFFIRM_FLAG);
				Log::Trace("","tmsm06_upd","ttmsm04.OPERATER [{0}]"		,ttmsm04.OPERATER);
				Log::Trace("","tmsm06_upd","ttmsm04.GRP_NO [{0}]"		,ttmsm04.GRP_NO);
				Log::Trace("","tmsm06_upd","ttmsm04.SHIFT_NO [{0}]"		,ttmsm04.SHIFT_NO);
				Log::Trace("","tmsm06_upd","ttmsm04.MAINTAIN_REMARK [{0}]"		,ttmsm04.MAINTAIN_REMARK);
				Log::Trace("","tmsm06_upd","ttmsm04.REMARK [{0}]"		,ttmsm04.REMARK);
				Log::Trace("","tmsm06_upd","ttmsm04.LADEL_LEVEL [{0}]"		,ttmsm04.LADEL_LEVEL);
				Log::Trace("","tmsm06_upd","ttmsm04.USE_SEQ_NO [{0}]"		,ttmsm04.USE_SEQ_NO);

				//根据传入信息更新钢包使用实绩表
				sqlstr = "UPDATE TTMSM04"; //用于捕获数据库操作异常情况
				ttmsm04.Update(	" REC_REVISOR "
								" ,REC_REVISE_TIME "
								" ,ARCHIVE_FLAG "			
								" ,SM_UNIT_NO "			
								" ,LADEL_LEVEL "
								" ,USE_SEQ_NO"
								" ,REPAIR_SEQ_NO "			
								" ,MAINT_TYPE "	
								" ,REPAIR_START_TIME "			
								" ,REPAIR_END_TIME "			
								" ,HOT_REPAIR_NUM "			
								" ,HOT_REPAIR_DATE "				
								" ,HOT_REPAIR_USE_NUM "				
								" ,REPAIR_STATION "	
								" ,IN_TIME "				
								" ,USE_LIFE1 "			
								" ,RESIDUAL_THICK1 "		
								" ,USE_LIFE2 "
								" ,RESIDUAL_THICK2 "	
								" ,USE_LIFE3 "	
								" ,RESIDUAL_THICK3 "	
								" ,USE_LIFE4 "	
								" ,RESIDUAL_THICK4 "			
								" ,USE_LIFE5 "				
								" ,RESIDUAL_THICK5 "
								" ,PACK_ALONG_HEIGHT "		
								" ,SAND_WT "	
								" ,SHELL_UP_TEMP "		
								" ,SHELL_DOWN_TEMP "		
								" ,RESIDUAL_STEEL_THICK "	
								" ,AFFIRM_FLAG "		
								" ,OPERATER "			
								" ,GRP_NO "	
								" ,SHIFT_NO "	
								" ,MAINTAIN_REMARK "				
								" ,REMARK "		
								" ,LADLE_NO ");
			}
		}
		else if(v_div.Trim() == "TMSM01")
		{
			/*重置TTMSM05*/
			ttmsm01.Reset();
			/*读取传入的参数*/
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm06_upd","ttmsm01.LADLE_NO [{0}]"		,(const char*)ttmsm01.LADLE_NO);
			Log::Trace("","tmsm06_upd","ttmsm01.SM_UNIT_NO [{0}]"	,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace("","tmsm06_upd","ttmsm01.USAGE_ST [{0}]"		,(const char*)ttmsm01.USAGE_ST);
			Log::Trace("","tmsm06_upd","ttmsm01.USAGE_ET [{0}]"		,(const char*)ttmsm01.USAGE_ET);

			// 判传入的参数是否有错			
			if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
			}
			if(ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
			}
			if(ttmsm01.USAGE_ST.Trim() == "")
			{
				sprintf	(s.msg , "传入的使用起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
			}
			if(ttmsm01.USAGE_ET.Trim() == "")
			{
				sprintf	(s.msg , "传入的使用结束时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
			}

			if(ttmsm04.HOT_REPAIR_DATE.Trim() != "")
			{
				ttmsm04.HOT_REPAIR_DATE = ttmsm04.HOT_REPAIR_DATE.Substring(0,8);
			}

			//班次班组 shift_no shift_group
			f_epep_get_shift_group("SM",ttmsm01.USAGE_ST,v_shift,v_group,conn );

			/*查询钢包使用实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM04 "
					 "  WHERE LADLE_NO		= @ttmsm04.LADLE_NO ";
					 
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm04.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm06_upd","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count == 0)
			{
				sprintf	(s.msg ,"钢包维修实绩信息不存在");
				/*新增钢包使用实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm06_upd");
			}
			
			/*修改钢包使用实绩表*/
			ttmsm04.CopyFrom(ttmsm01);
			//ttmsm04.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			//ttmsm04.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm04.REC_REVISOR = s.userid;
			ttmsm04.REC_REVISE_TIME = datetimeNow;
			//班次班组 shift_no shift_group

			//s.datetime是前台终端操作系统的时间，而不是服务器时间
			if(ttmsm04.GRP_NO.Trim() == "")
			{
				ttmsm04.GRP_NO = v_group;
			}
			if(ttmsm04.SHIFT_NO.Trim() == "")
			{
				ttmsm04.SHIFT_NO = v_shift;
			}
			ttmsm04.TrimOrBlank();
			sqlstr = "UPDATE TTMSM04"; //用于捕获数据库操作异常情况
			ttmsm04.Update(	" REC_REVISOR "
								" ,REC_REVISE_TIME "
								" ,ARCHIVE_FLAG "			
								" ,SM_UNIT_NO "			
								" ,LADEL_LEVEL "			
								" ,USE_SEQ_NO "	
								" ,REPAIR_SEQ_NO "			
								" ,MAINT_TYPE "	
								" ,REPAIR_START_TIME "			
								" ,REPAIR_END_TIME "			
								" ,HOT_REPAIR_NUM "			
								" ,HOT_REPAIR_DATE "				
								" ,HOT_REPAIR_USE_NUM "				
								" ,REPAIR_STATION "	
								" ,IN_TIME "				
								" ,USE_LIFE1 "			
								" ,RESIDUAL_THICK1 "		
								" ,USE_LIFE2 "
								" ,RESIDUAL_THICK2 "	
								" ,USE_LIFE3 "	
								" ,RESIDUAL_THICK3 "	
								" ,USE_LIFE4 "	
								" ,RESIDUAL_THICK4 "			
								" ,USE_LIFE5 "				
								" ,RESIDUAL_THICK5 "
								" ,PACK_ALONG_HEIGHT "		
								" ,SAND_WT "	
								" ,SHELL_UP_TEMP "		
								" ,SHELL_DOWN_TEMP "		
								" ,RESIDUAL_STEEL_THICK "	
								" ,AFFIRM_FLAG "		
								" ,OPERATER "			
								" ,GRP_NO "	
								" ,SHIFT_NO "	
								" ,MAINTAIN_REMARK "				
								" ,REMARK "			
								" ,LADLE_NO ");

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