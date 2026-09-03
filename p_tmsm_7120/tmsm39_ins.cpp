/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-22
功能: 结晶器使用实绩新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm39.h"
#include "ttmsm31.h"

/*<remark>=========================================================
/// <summary>
/// 结晶器使用实绩新增
/// <para>
/// 1.接收传入的结晶器使用实绩信息；
/// 2.新增结晶器使用实绩信息；
/// </para>
/// <para>数据库表：TTMSM39(结晶器使用实绩表)      </para>
/// <para>主调用函数：前台TTMSM31画面使用实绩新增调用；TTMSM33画面F3(新增)调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的结晶器使用实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm39_ins)

//void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm39_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM39 ttmsm39(conn);
	CTTMSM31 ttmsm31(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		//记录总条数
		int	rows = bcls_rec->Tables[0].Rows.get_Count(); 
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm39_ins","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM33")
		{
			for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
			{
				/*重置TTMSM39*/
				ttmsm39.Reset();
				/*读取传入的参数*/
				ttmsm39.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);

				Log::Trace("","tmsm39_ins","ttmsm39.MOLD_NO [{0}]"		,(const char*)ttmsm39.MOLD_NO);
				Log::Trace("","tmsm39_ins","ttmsm39.SM_UNIT_NO [{0}]"	,(const char*)ttmsm39.SM_UNIT_NO);
				Log::Trace("","tmsm39_ins","ttmsm39.ON_LINE_TIME [{0}]"	,(const char*)ttmsm39.ON_LINE_TIME);
				Log::Trace("","tmsm39_ins","ttmsm39.OFF_LINE_TIME [{0}]",(const char*)ttmsm39.OFF_LINE_TIME);
				Log::Trace("","tmsm39_ins","ttmsm39.MOLD_SECTION [{0}]"	,(const char*)ttmsm39.MOLD_SECTION);
				Log::Trace("","tmsm39_ins","ttmsm39.CC_MACH_NO [{0}]"	,(const char*)ttmsm39.CC_MACH_NO);

				//班次班组 shift_no shift_group
				//f_epep_get_shift_group("DEFAULT",ttmsm39.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错			
				if(ttmsm39.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}
				if(ttmsm39.MOLD_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的结晶器号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}
				if(ttmsm39.MOLD_SECTION.Trim() == "")
				{
					sprintf	(s.msg , "传入的结晶器断面不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}
				if(ttmsm39.CC_MACH_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的结晶器所在连铸机号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}
				if(ttmsm39.ON_LINE_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的上线时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}
				if(ttmsm39.OFF_LINE_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的下线时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}

				/*查询结晶器使用实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM39 "
						 "  WHERE SM_UNIT_NO	= @ttmsm39.SM_UNIT_NO "
						 "  AND ON_LINE_TIME	= @ttmsm39.ON_LINE_TIME "						 
						 "  AND MOLD_SECTION	= @ttmsm39.MOLD_SECTION "
						 "  AND CC_MACH_NO		= @ttmsm39.CC_MACH_NO "
						 "  AND MOLD_NO			= @ttmsm39.MOLD_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm39.SM_UNIT_NO"	, ttmsm39.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm39.ON_LINE_TIME"	, ttmsm39.ON_LINE_TIME );
				cmd_inq.Parameters.Set( "ttmsm39.MOLD_NO"		, ttmsm39.MOLD_NO );
				cmd_inq.Parameters.Set( "ttmsm39.MOLD_SECTION"	, ttmsm39.MOLD_SECTION );
				cmd_inq.Parameters.Set( "ttmsm39.CC_MACH_NO"	, ttmsm39.CC_MACH_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if	(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
					Log::Trace("","tmsm39_ins","count {0}",count );
				}
				cmd_inq.Close();
				if(count >= 1)
				{
					sprintf	(s.msg ,"结晶器使用实绩信息已存在");
					/*新增结晶器使用实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
				}
				
				// 生成使用流水号BAKE_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
							" FROM TTMSM39 "
							" WHERE SM_UNIT_NO	= @ttmsm39.SM_UNIT_NO "							
							" AND MOLD_SECTION 	= @ttmsm39.MOLD_SECTION "
							" AND CC_MACH_NO	= @ttmsm39.CC_MACH_NO "
							" AND MOLD_NO 		= @ttmsm39.MOLD_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm39.SM_UNIT_NO"		, ttmsm39.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm39.MOLD_NO"		, ttmsm39.MOLD_NO);
				cmd_inq.Parameters.Set("ttmsm39.MOLD_SECTION"	, ttmsm39.MOLD_SECTION);
				cmd_inq.Parameters.Set("ttmsm39.CC_MACH_NO"		, ttmsm39.CC_MACH_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm39.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","tmsm39_ins","ttmsm39.USE_SEQ_NO [{0}]",ttmsm39.USE_SEQ_NO);

				/*新增结晶器使用实绩表*/
				ttmsm39.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				ttmsm39.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 
				//s.datetime是前台终端操作系统的时间，而不是服务器时间

				ttmsm39.TrimOrBlank();
				ttmsm39.Insert();
			}
		}
		else if(v_div.Trim() == "TMSM31")
		{
			/*重置TTMSM31*/
			ttmsm31.Reset();
			/*读取传入的参数*/
			ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			ttmsm31.Print();

			Log::Trace("","tmsm39_ins","ttmsm31.MOLD_NO [{0}]"		,(const char*)ttmsm31.MOLD_NO);
			Log::Trace("","tmsm39_ins","ttmsm31.MOLD_SECTION [{0}]"	,(const char*)ttmsm31.MOLD_SECTION);
			Log::Trace("","tmsm39_ins","ttmsm31.CC_MACH_NO [{0}]"	,(const char*)ttmsm31.CC_MACH_NO);
			Log::Trace("","tmsm39_ins","ttmsm31.SM_UNIT_NO [{0}]"	,(const char*)ttmsm31.SM_UNIT_NO);
			Log::Trace("","tmsm39_ins","ttmsm31.ON_LINE_TIME [{0}]"	,(const char*)ttmsm31.ON_LINE_TIME);
			Log::Trace("","tmsm39_ins","ttmsm31.OFF_LINE_TIME [{0}]",(const char*)ttmsm31.OFF_LINE_TIME);

			// 判传入的参数是否有错			
			if(ttmsm31.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
			}
			if(ttmsm31.MOLD_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
			}
			if(ttmsm31.MOLD_SECTION.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器断面不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
			}
			if(ttmsm31.CC_MACH_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的连铸机号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
			}
			if(ttmsm31.ON_LINE_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的上线时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
			}

			/*查询结晶器使用实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM39 "
					 "  WHERE SM_UNIT_NO	= @ttmsm39.SM_UNIT_NO "
					 "  AND ON_LINE_TIME	= @ttmsm39.ON_LINE_TIME "
					 "  AND MOLD_SECTION	= @ttmsm39.MOLD_SECTION "
					 "  AND CC_MACH_NO		= @ttmsm39.CC_MACH_NO "
					 "  AND MOLD_NO			= @ttmsm39.MOLD_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm39.SM_UNIT_NO"	, ttmsm31.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm39.ON_LINE_TIME"	, ttmsm31.ON_LINE_TIME );
			cmd_inq.Parameters.Set( "ttmsm39.MOLD_SECTION"	, ttmsm31.MOLD_SECTION );
			cmd_inq.Parameters.Set( "ttmsm39.CC_MACH_NO"	, ttmsm31.CC_MACH_NO );
			cmd_inq.Parameters.Set( "ttmsm39.MOLD_NO"		, ttmsm31.MOLD_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm39_ins","count {0}",count );
			}
			cmd_inq.Close();
			if(count >= 1)
			{
				sprintf	(s.msg ,"结晶器使用实绩信息已存在");
				/*新增结晶器使用实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm39_ins");
			}
			
			// 生成使用流水号REPAIR_SEQ_NO
			sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
						" FROM TTMSM39 "
						" WHERE SM_UNIT_NO	= @ttmsm39.SM_UNIT_NO "
						" AND MOLD_SECTION	= @ttmsm39.MOLD_SECTION "
					    " AND CC_MACH_NO	= @ttmsm39.CC_MACH_NO "
						" AND MOLD_NO		= @ttmsm39.MOLD_NO "
						;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("ttmsm39.SM_UNIT_NO"		, ttmsm31.SM_UNIT_NO);
			cmd_inq.Parameters.Set("ttmsm39.MOLD_SECTION"	, ttmsm31.MOLD_SECTION);
			cmd_inq.Parameters.Set("ttmsm39.CC_MACH_NO"		, ttmsm31.CC_MACH_NO);
			cmd_inq.Parameters.Set("ttmsm39.MOLD_NO"		, ttmsm31.MOLD_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				ttmsm31.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
			}
			cmd_inq.Close();
			Log::Trace("","tmsm39_ins","ttmsm31.USE_SEQ_NO [{0}]",ttmsm31.USE_SEQ_NO);

			/*新增结晶器使用实绩表*/
			ttmsm39.CopyFrom(ttmsm31);
			ttmsm39.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			ttmsm39.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm39.REC_REVISOR = " ";
			ttmsm39.REC_REVISE_TIME = " ";

			ttmsm39.TrimOrBlank();
			ttmsm39.Insert();

			sqlstr = "UPDATE TTMSM31";
			ttmsm31.CURRENT_STATUS = "11"; // 01-可用 11-使用 21-维修 99-报废
			ttmsm31.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,CURRENT_STATUS "		//当前状态
							" ,CC_MACH_NO "			//连铸机号
							" ,STRAND_NO "			//流号
							" ,PROD_DATE "			//生产日期
							" ,START_USE_DATE "		//开始启用日期
							" ,USE_SEQ_NO "			//使用流水号
							" ,COPPER_CHANGE_DIV "  //更换铜管板标记
							" ,MD_COPPER_PLATE_NO " //铜管板号
							" ,COPPER_MANU "		//铜板/管厂家
							" ,COPPER_TIMES "		//铜板/管使用次数
							" ,ON_LINE_TIME "		//上线时间
							" ,OFF_LINE_TIME "		//下线时间
							" ,UNLADE_REASON "		//下线原因
							" ,CAST_NO "			//连连浇号(CAST号)
							" ,ST_NO "				//出钢记号
							" ,MOLD_NUM "			//结晶器使用炉数
							" ,TOTAL_CHARGE_NUM "   //累计总炉数
							" ,STD_CHARGE_NUM "		//标准使用炉数
							" ,CUR_CAST_WT "		//当前浇铸重量
							" ,CUR_CAST_LEN "		//当前浇铸长度
							" ,TOTAL_CAST_WT "		//总浇铸重量
							" ,TOTAL_CAST_LEN "		//总浇铸长度
							" ,USE_REMARK "			//使用备注
							" ,COPPER_TIMES_1 "		//铜板1使用次数 内外左右
							" ,COPPER_TIMES_2 "		//铜板2使用次数
							" ,COPPER_TIMES_3 "		//铜板3使用次数
							" ,COPPER_TIMES_4 ",	//铜板4使用次数
							" SM_UNIT_NO,MOLD_NO,CC_MACH_NO,MOLD_SECTION ");
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