import java.awt.*;
import javax.swing.*;

/**
 * <p>
 * Title:
 * </p>
 * <p>
 * Description:
 * </p>
 * <p>
 * Copyright: Copyright (c) 2003
 * </p>
 * <p>
 * Company:
 * </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class RFXPRConfigV1Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	DeviceField deviceField3 = new DeviceField();
	BorderLayout borderLayout30 = new BorderLayout();
	JPanel jPanel30 = new JPanel();
	JPanel jPanel26 = new JPanel();
	JTabbedPane jTabbedPane3 = new JTabbedPane();
	BorderLayout borderLayout28 = new BorderLayout();
	BorderLayout borderLayout21 = new BorderLayout();
	DeviceTable deviceTable115 = new DeviceTable();
	JPanel jPanel27 = new JPanel();
	BorderLayout borderLayout211 = new BorderLayout();
	DeviceTable deviceTable1110 = new DeviceTable();
	BorderLayout borderLayout22 = new BorderLayout();
	BorderLayout borderLayout25 = new BorderLayout();
	JPanel jPanel215 = new JPanel();
	BorderLayout borderLayout215 = new BorderLayout();
	DeviceTable deviceTable111 = new DeviceTable();
	JPanel jPanel28 = new JPanel();
	DeviceTable deviceTable110 = new DeviceTable();
	BorderLayout borderLayout26 = new BorderLayout();
	DeviceTable deviceTable1111 = new DeviceTable();
	JPanel jPanel214 = new JPanel();
	BorderLayout borderLayout214 = new BorderLayout();
	JPanel jPanel213 = new JPanel();
	BorderLayout borderLayout19 = new BorderLayout();
	BorderLayout borderLayout29 = new BorderLayout();
	JPanel jPanel211 = new JPanel();
	DeviceTable deviceTable113 = new DeviceTable();
	JPanel jPanel25 = new JPanel();
	BorderLayout borderLayout210 = new BorderLayout();
	DeviceTable deviceTable116 = new DeviceTable();
	JPanel jPanel3 = new JPanel();
	BorderLayout borderLayout20 = new BorderLayout();
	JPanel jPanel24 = new JPanel();
	BorderLayout borderLayout27 = new BorderLayout();
	JPanel jPanel210 = new JPanel();
	DeviceTable deviceTable114 = new DeviceTable();
	JPanel jPanel212 = new JPanel();
	DeviceTable deviceTable117 = new DeviceTable();
	JPanel jPanel22 = new JPanel();
	DeviceTable deviceTable112 = new DeviceTable();
	JPanel jPanel21 = new JPanel();
	DeviceTable deviceTable18 = new DeviceTable();
	BorderLayout borderLayout23 = new BorderLayout();
	DeviceTable deviceTable19 = new DeviceTable();
	JPanel jPanel29 = new JPanel();
	DeviceTable deviceTable119 = new DeviceTable();
	JPanel jPanel20 = new JPanel();
	DeviceTable deviceTable17 = new DeviceTable();
	BorderLayout borderLayout24 = new BorderLayout();
	DeviceTable deviceTable118 = new DeviceTable();
	JPanel jPanel23 = new JPanel();
	BorderLayout borderLayout212 = new BorderLayout();
	DeviceTable deviceTable1112 = new DeviceTable();
	BorderLayout borderLayout213 = new BorderLayout();
	JPanel jPanel16 = new JPanel();
	BorderLayout borderLayout15 = new BorderLayout();
	JPanel jPanel19 = new JPanel();
	DeviceTable deviceTable10 = new DeviceTable();
	DeviceTable deviceTable6 = new DeviceTable();
	JPanel jPanel13 = new JPanel();
	BorderLayout borderLayout14 = new BorderLayout();
	DeviceTable deviceTable9 = new DeviceTable();
	JPanel jPanel10 = new JPanel();
	BorderLayout borderLayout8 = new BorderLayout();
	BorderLayout borderLayout7 = new BorderLayout();
	JPanel jPanel12 = new JPanel();
	DeviceTable deviceTable3 = new DeviceTable();
	JTabbedPane jTabbedPane2 = new JTabbedPane();
	DeviceTable deviceTable1 = new DeviceTable();
	DeviceTable deviceTable16 = new DeviceTable();
	JPanel jPanel7 = new JPanel();
	DeviceTable deviceTable11 = new DeviceTable();
	BorderLayout borderLayout12 = new BorderLayout();
	DeviceTable deviceTable5 = new DeviceTable();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel14 = new JPanel();
	DeviceTable deviceTable8 = new DeviceTable();
	DeviceTable deviceTable7 = new DeviceTable();
	JPanel jPanel4 = new JPanel();
	BorderLayout borderLayout17 = new BorderLayout();
	DeviceTable deviceTable15 = new DeviceTable();
	BorderLayout borderLayout10 = new BorderLayout();
	BorderLayout borderLayout4 = new BorderLayout();
	DeviceTable deviceTable14 = new DeviceTable();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel5 = new JPanel();
	BorderLayout borderLayout13 = new BorderLayout();
	JPanel jPanel11 = new JPanel();
	DeviceTable deviceTable4 = new DeviceTable();
	BorderLayout borderLayout16 = new BorderLayout();
	DeviceTable deviceTable12 = new DeviceTable();
	DeviceTable deviceTable13 = new DeviceTable();
	JPanel jPanel8 = new JPanel();
	BorderLayout borderLayout18 = new BorderLayout();
	BorderLayout borderLayout2 = new BorderLayout();
	BorderLayout borderLayout11 = new BorderLayout();
	BorderLayout borderLayout3 = new BorderLayout();
	JPanel jPanel18 = new JPanel();
	JPanel jPanel17 = new JPanel();
	DeviceTable deviceTable2 = new DeviceTable();
	BorderLayout borderLayout9 = new BorderLayout();
	BorderLayout borderLayout5 = new BorderLayout();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel9 = new JPanel();
	BorderLayout borderLayout6 = new BorderLayout();
	JPanel jPanel31 = new JPanel();
	BorderLayout borderLayout31 = new BorderLayout();
	JScrollPane jScrollPane1 = new JScrollPane();
	DeviceTable deviceTable20 = new DeviceTable();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel32 = new JPanel();
	JPanel jPanel33 = new JPanel();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();

	public RFXPRConfigV1Setup()
	{
		try
		{
			jbInit();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		this.setWidth(682);
		this.setHeight(682);
		this.setDeviceType("RFXPRConfig");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("RFX PR Configuration");
		this.getContentPane().setLayout(borderLayout1);
		deviceField3.setOffsetNid(1);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Comment: ");
		deviceField3.setNumCols(40);
		deviceField3.setIdentifier("");
		jPanel1.setLayout(borderLayout30);
		jPanel26.setLayout(borderLayout26);
		deviceTable115.setOffsetNid(18);
		deviceTable115.setNumCols(12);
		deviceTable115.setNumRows(14);
		deviceTable115.setIdentifier("");
		deviceTable115.setColumnNames(new String[]
		{ "97", "98", "99", "100", "101", "102", "103", "104", "105", "106", "107", "108" });
		deviceTable115.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable115.setDisplayRowNumber(true);
		jPanel27.setLayout(borderLayout27);
		deviceTable1110.setOffsetNid(23);
		deviceTable1110.setNumCols(12);
		deviceTable1110.setNumRows(14);
		deviceTable1110.setIdentifier("");
		deviceTable1110.setColumnNames(new String[]
		{ "157", "158", "159", "160", "161", "162", "163", "164", "165", "16", "167", "168" });
		deviceTable1110.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable1110.setDisplayRowNumber(true);
		jPanel215.setLayout(borderLayout215);
		deviceTable111.setOffsetNid(14);
		deviceTable111.setNumCols(12);
		deviceTable111.setNumRows(14);
		deviceTable111.setIdentifier("");
		deviceTable111.setColumnNames(new String[]
		{ "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60" });
		deviceTable111.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable111.setDisplayRowNumber(true);
		jPanel28.setLayout(borderLayout28);
		deviceTable110.setOffsetNid(13);
		deviceTable110.setNumCols(12);
		deviceTable110.setNumRows(14);
		deviceTable110.setIdentifier("");
		deviceTable110.setColumnNames(new String[]
		{ "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48" });
		deviceTable110.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable110.setDisplayRowNumber(true);
		deviceTable1111.setOffsetNid(24);
		deviceTable1111.setNumCols(12);
		deviceTable1111.setNumRows(14);
		deviceTable1111.setIdentifier("");
		deviceTable1111.setColumnNames(new String[]
		{ "169", "170", "171", "172", "173", "174", "175", "175", "177", "178", "179", "180" });
		deviceTable1111.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable1111.setDisplayRowNumber(true);
		jPanel214.setLayout(borderLayout214);
		jPanel213.setLayout(borderLayout213);
		jPanel211.setLayout(borderLayout211);
		deviceTable113.setOffsetNid(16);
		deviceTable113.setNumCols(12);
		deviceTable113.setNumRows(14);
		deviceTable113.setIdentifier("");
		deviceTable113.setColumnNames(new String[]
		{ "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84" });
		deviceTable113.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable113.setDisplayRowNumber(true);
		jPanel25.setLayout(borderLayout25);
		deviceTable116.setOffsetNid(19);
		deviceTable116.setNumCols(12);
		deviceTable116.setNumRows(14);
		deviceTable116.setIdentifier("");
		deviceTable116.setColumnNames(new String[]
		{ "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120" });
		deviceTable116.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable116.setDisplayRowNumber(true);
		jPanel3.setLayout(borderLayout19);
		jPanel24.setLayout(borderLayout24);
		jPanel210.setLayout(borderLayout210);
		deviceTable114.setOffsetNid(17);
		deviceTable114.setNumCols(12);
		deviceTable114.setNumRows(14);
		deviceTable114.setIdentifier("");
		deviceTable114.setColumnNames(new String[]
		{ "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96" });
		deviceTable114.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable114.setDisplayRowNumber(true);
		jPanel212.setLayout(borderLayout212);
		deviceTable117.setOffsetNid(20);
		deviceTable117.setNumCols(12);
		deviceTable117.setNumRows(14);
		deviceTable117.setIdentifier("");
		deviceTable117.setColumnNames(new String[]
		{ "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131", "132" });
		deviceTable117.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable117.setDisplayRowNumber(true);
		jPanel22.setLayout(borderLayout22);
		deviceTable112.setOffsetNid(15);
		deviceTable112.setNumCols(12);
		deviceTable112.setNumRows(14);
		deviceTable112.setIdentifier("");
		deviceTable112.setColumnNames(new String[]
		{ "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72" });
		deviceTable112.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable112.setDisplayRowNumber(true);
		jPanel21.setLayout(borderLayout21);
		deviceTable18.setOffsetNid(11);
		deviceTable18.setNumCols(12);
		deviceTable18.setNumRows(14);
		deviceTable18.setIdentifier("");
		deviceTable18.setColumnNames(new String[]
		{ "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24" });
		deviceTable18.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable18.setDisplayRowNumber(true);
		deviceTable19.setOffsetNid(12);
		deviceTable19.setNumCols(12);
		deviceTable19.setNumRows(14);
		deviceTable19.setIdentifier("");
		deviceTable19.setColumnNames(new String[]
		{ "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36" });
		deviceTable19.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable19.setDisplayRowNumber(true);
		jPanel29.setLayout(borderLayout29);
		deviceTable119.setOffsetNid(22);
		deviceTable119.setNumCols(12);
		deviceTable119.setNumRows(14);
		deviceTable119.setIdentifier("");
		deviceTable119.setColumnNames(new String[]
		{ "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156" });
		deviceTable119.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable119.setDisplayRowNumber(true);
		jPanel20.setLayout(borderLayout20);
		deviceTable17.setOffsetNid(10);
		deviceTable17.setNumCols(12);
		deviceTable17.setNumRows(14);
		deviceTable17.setIdentifier("");
		deviceTable17.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" });
		deviceTable17.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable17.setDisplayRowNumber(true);
		deviceTable118.setOffsetNid(21);
		deviceTable118.setNumCols(12);
		deviceTable118.setNumRows(14);
		deviceTable118.setIdentifier("");
		deviceTable118.setColumnNames(new String[]
		{ "133", "134", "135", "136", "137", "138", "139", "140", "141", "142", "143", "144" });
		deviceTable118.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable118.setDisplayRowNumber(true);
		jPanel23.setLayout(borderLayout23);
		deviceTable1112.setOffsetNid(25);
		deviceTable1112.setNumCols(12);
		deviceTable1112.setNumRows(14);
		deviceTable1112.setIdentifier("");
		deviceTable1112.setColumnNames(new String[]
		{ "181", "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192" });
		deviceTable1112.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "Selezione PWM", "Limite_corrente", "Amp_rif_interno",
				"Freq_rif_interno", "Sfasamento", "Offset_rif_interno" });
		deviceTable1112.setDisplayRowNumber(true);
		jPanel16.setLayout(borderLayout15);
		jPanel19.setLayout(borderLayout18);
		deviceTable10.setOffsetNid(36);
		deviceTable10.setNumCols(12);
		deviceTable10.setNumRows(45);
		deviceTable10.setIdentifier("");
		deviceTable10.setColumnNames(new String[]
		{ "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120" });
		deviceTable10.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable10.setEditable(false);
		deviceTable10.setDisplayRowNumber(true);
		deviceTable6.setOffsetNid(32);
		deviceTable6.setNumCols(12);
		deviceTable6.setNumRows(45);
		deviceTable6.setIdentifier("");
		deviceTable6.setColumnNames(new String[]
		{ "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72" });
		deviceTable6.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable6.setEditable(false);
		deviceTable6.setDisplayRowNumber(true);
		jPanel13.setLayout(borderLayout12);
		deviceTable9.setOffsetNid(35);
		deviceTable9.setNumCols(12);
		deviceTable9.setNumRows(45);
		deviceTable9.setIdentifier("");
		deviceTable9.setColumnNames(new String[]
		{ "97", "98", "99", "100", "101", "102", "103", "104", "105", "106", "107", "108" });
		deviceTable9.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable9.setEditable(false);
		deviceTable9.setDisplayRowNumber(true);
		jPanel10.setLayout(borderLayout9);
		jPanel12.setLayout(borderLayout11);
		deviceTable3.setOffsetNid(29);
		deviceTable3.setNumCols(12);
		deviceTable3.setNumRows(45);
		deviceTable3.setIdentifier("");
		deviceTable3.setColumnNames(new String[]
		{ "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36" });
		deviceTable3.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable3.setEditable(false);
		deviceTable3.setDisplayRowNumber(true);
		deviceTable1.setOffsetNid(27);
		deviceTable1.setNumCols(12);
		deviceTable1.setNumRows(45);
		deviceTable1.setIdentifier("");
		deviceTable1.setColumnNames(new String[]
		{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" });
		deviceTable1.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable1.setEditable(false);
		deviceTable1.setDisplayRowNumber(true);
		deviceTable16.setOffsetNid(42);
		deviceTable16.setNumCols(12);
		deviceTable16.setNumRows(45);
		deviceTable16.setIdentifier("");
		deviceTable16.setColumnNames(new String[]
		{ "181", "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192" });
		deviceTable16.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable16.setEditable(false);
		deviceTable16.setDisplayRowNumber(true);
		jPanel7.setLayout(borderLayout6);
		deviceTable11.setOffsetNid(37);
		deviceTable11.setNumCols(12);
		deviceTable11.setNumRows(45);
		deviceTable11.setIdentifier("");
		deviceTable11.setColumnNames(new String[]
		{ "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131", "132" });
		deviceTable11.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable11.setEditable(false);
		deviceTable11.setDisplayRowNumber(true);
		deviceTable5.setOffsetNid(31);
		deviceTable5.setNumCols(12);
		deviceTable5.setNumRows(45);
		deviceTable5.setIdentifier("");
		deviceTable5.setColumnNames(new String[]
		{ "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60" });
		deviceTable5.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable5.setEditable(false);
		deviceTable5.setDisplayRowNumber(true);
		jPanel6.setLayout(borderLayout5);
		jPanel14.setLayout(borderLayout13);
		deviceTable8.setOffsetNid(34);
		deviceTable8.setNumCols(12);
		deviceTable8.setNumRows(45);
		deviceTable8.setIdentifier("");
		deviceTable8.setColumnNames(new String[]
		{ "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96" });
		deviceTable8.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable8.setEditable(false);
		deviceTable8.setDisplayRowNumber(true);
		deviceTable7.setOffsetNid(33);
		deviceTable7.setNumCols(12);
		deviceTable7.setNumRows(45);
		deviceTable7.setIdentifier("");
		deviceTable7.setColumnNames(new String[]
		{ "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84" });
		deviceTable7.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable7.setEditable(false);
		deviceTable7.setDisplayRowNumber(true);
		jPanel4.setLayout(borderLayout3);
		deviceTable15.setOffsetNid(41);
		deviceTable15.setNumCols(12);
		deviceTable15.setNumRows(45);
		deviceTable15.setIdentifier("");
		deviceTable15.setColumnNames(new String[]
		{ "169", "170", "171", "172", "173", "174", "175", "176", "177", "178", "179", "180" });
		deviceTable15.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable15.setEditable(false);
		deviceTable15.setDisplayRowNumber(true);
		deviceTable14.setOffsetNid(40);
		deviceTable14.setNumCols(12);
		deviceTable14.setNumRows(45);
		deviceTable14.setIdentifier("");
		deviceTable14.setColumnNames(new String[]
		{ "157", "158", "159", "160", "161", "162", "163", "164", "165", "166", "167", "168" });
		deviceTable14.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable14.setEditable(false);
		deviceTable14.setDisplayRowNumber(true);
		jPanel2.setLayout(borderLayout2);
		jPanel5.setLayout(borderLayout4);
		jPanel11.setLayout(borderLayout10);
		deviceTable4.setOffsetNid(30);
		deviceTable4.setNumCols(12);
		deviceTable4.setNumRows(45);
		deviceTable4.setIdentifier("");
		deviceTable4.setColumnNames(new String[]
		{ "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48" });
		deviceTable4.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable4.setEditable(false);
		deviceTable4.setDisplayRowNumber(true);
		deviceTable12.setOffsetNid(38);
		deviceTable12.setNumCols(12);
		deviceTable12.setNumRows(45);
		deviceTable12.setIdentifier("");
		deviceTable12.setColumnNames(new String[]
		{ "133", "134", "135", "136", "137", "138", "139", "140", "141", "142", "143", "144" });
		deviceTable12.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable12.setEditable(false);
		deviceTable12.setDisplayRowNumber(true);
		deviceTable13.setOffsetNid(39);
		deviceTable13.setNumCols(12);
		deviceTable13.setNumRows(45);
		deviceTable13.setIdentifier("");
		deviceTable13.setColumnNames(new String[]
		{ "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156" });
		deviceTable13.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable13.setEditable(false);
		deviceTable13.setDisplayRowNumber(true);
		jPanel8.setLayout(borderLayout7);
		jPanel18.setLayout(borderLayout17);
		jPanel17.setLayout(borderLayout16);
		deviceTable2.setOffsetNid(28);
		deviceTable2.setNumCols(12);
		deviceTable2.setNumRows(46);
		deviceTable2.setIdentifier("");
		deviceTable2.setColumnNames(new String[]
		{ "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24" });
		deviceTable2.setRowNames(new String[]
		{ "Start_seriale", "Anello_corrente", "Anello_flusso", "Anello_aperto", "Rif_interno", "Rif_sinusoidale",
				"Retr_tens_uscita", "Rif_continuo", "PWM_5kHz", "Not_used", "Corr_massima", "Corr_nominale",
				"FS_lett_tens_barra", "Tens_max_barra", "Tens_min_barra", "Maschera_uscita_KD", "Maschera_uscita_KE",
				"Offset_comp_tens_V0", "Resist_int_IGBT", "Corrente_minima", "Limite_corrente", "FS_offset_tensione",
				"Res_interna_massima", "Temp_termica", "K_adatt_limite_flusso", "K_adatt_windup", "K_integ_flusso",
				"K_A", "K_prop_flusso", "K_adatt_rif_corrente", "K_adatt_rif_flusso", "K_PI", "K_II", "K_AI",
				"KC_anello_flusso", "KB_anello_flusso", "KC_anello_corrente", "KB_anello_corrente",
				"KP_comp_anello_flusso", "KP_comp_anello_corr", "KP_adj_riferimento", "Amp_riferimento_int",
				"Freq_riferimento_int", "Sfasamento", "Offset_val_rif_int" });
		deviceTable2.setEditable(false);
		deviceTable2.setDisplayRowNumber(true);
		jPanel15.setLayout(borderLayout14);
		jPanel9.setLayout(borderLayout8);
		jPanel31.setLayout(borderLayout31);
		deviceTable20.setOffsetNid(26);
		deviceTable20.setLabelString("Chopper ON/OFF");
		deviceTable20.setNumCols(4);
		deviceTable20.setNumRows(48);
		deviceTable20.setIdentifier("");
		deviceTable20.setRowNames(new String[]
		{ "1(1,2,9,10)", "2(3,4,11,12)", "3(5,6,13,14)", "4(7,8,15,16)", "5(17,18,25,26)", "6(19,20,27,28)",
				"7(21,22,29,30)", "8(23,24,31,32)", "9(33,34,41,42)", "10(35,36,43,44)", "11(37,38,45,46)",
				"12(39,40,47,48)", "13(49,50,57,58)", "14(51,52,59,60)", "15(53,54,61,62)", "16(55,56,63,64)",
				"17(65,66,73,74)", "18(67,68,75,76)", "19(69,70,77,78)", "20(71,72,79,80)", "21(81,82,89,90)",
				"22(83,84,91,92)", "23(85,86,93,94)", "24(87,88,95,96)", "25(97,98,105,106)", "26(99,100,107,108)",
				"27(101,102,109,110)", "28(103,104,111,112)", "29(113,114,121,122)", "30(115,116,123,124)",
				"31(117,118,125,126)", "32(119,120,127,128)", "33(129,130,137,138)", "34(131,132,139,140)",
				"35(133,134,141,142)", "36(135,136,143,144)", "37(145,146,153,154)", "38(147,148,155,156)",
				"39(149,150,157,158)", "40(151,152,159,160)", "41(161,162,169,170)", "42(163,164,171,172)",
				"43(165,166,173,174)", "44(167,168,175,176)", "45(177,178,185,186)", "46(179,180,187,188)",
				"47(181,182,189,190)", "48(183,184,191,192)" });
		deviceTable20.setColumnNames(new String[]
		{ "EXTERNAL", "TOP", "INTERNAL", "BOTTOM" });
		deviceTable20.setBinary(true);
		deviceTable20.setDisplayRowNumber(true);
		deviceTable20.setPreferredColumnWidth(70);
		deviceTable20.setPreferredHeight(85);
		jPanel30.setLayout(gridLayout1);
		gridLayout1.setRows(2);
		deviceField2.setOffsetNid(8);
		deviceField2.setLabelString("Bar Volt.4:");
		deviceField2.setNumCols(6);
		deviceField2.setIdentifier("");
		deviceField1.setOffsetNid(9);
		deviceField1.setLabelString("Bar Curr.4:");
		deviceField1.setNumCols(6);
		deviceField1.setIdentifier("");
		deviceField6.setOffsetNid(2);
		deviceField6.setLabelString("Bar Volt.1:");
		deviceField6.setNumCols(6);
		deviceField6.setIdentifier("");
		deviceField5.setOffsetNid(4);
		deviceField5.setLabelString("bar Volt.2:");
		deviceField5.setNumCols(6);
		deviceField5.setIdentifier("");
		deviceField4.setOffsetNid(6);
		deviceField4.setLabelString("Bar Volt.3:");
		deviceField4.setNumCols(6);
		deviceField4.setIdentifier("");
		deviceField9.setOffsetNid(3);
		deviceField9.setLabelString("Bar Curr.1:");
		deviceField9.setNumCols(6);
		deviceField9.setIdentifier("");
		deviceField8.setOffsetNid(5);
		deviceField8.setLabelString("Bar Curr.2:");
		deviceField8.setNumCols(6);
		deviceField8.setIdentifier("");
		deviceField7.setOffsetNid(7);
		deviceField7.setLabelString("Bar Curr.3:");
		deviceField7.setNumCols(6);
		deviceField7.setIdentifier("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(deviceField3, BorderLayout.NORTH);
		jPanel1.add(jPanel30, BorderLayout.CENTER);
		jPanel30.add(jPanel33, null);
		jPanel33.add(deviceField6, null);
		jPanel33.add(deviceField5, null);
		jPanel33.add(deviceField4, null);
		jPanel33.add(deviceField2, null);
		jPanel30.add(jPanel32, null);
		jPanel32.add(deviceField9, null);
		jPanel32.add(deviceField8, null);
		jPanel32.add(deviceField7, null);
		jPanel32.add(deviceField1, null);
		this.getContentPane().add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel3, "Sent Parameters");
		jPanel3.add(jTabbedPane3, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel20, "Chop 1-12");
		jPanel20.add(deviceTable17, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel21, "Chop 13-24");
		jPanel21.add(deviceTable18, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel22, "Chop 25-36");
		jPanel22.add(deviceTable19, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel23, "Chop 37-48");
		jPanel23.add(deviceTable110, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel24, "Chop 49-60");
		jPanel24.add(deviceTable111, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel25, "Chop 61-72");
		jPanel25.add(deviceTable112, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel26, "Chop 73-84");
		jPanel26.add(deviceTable113, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel27, "Chop 85-96");
		jPanel27.add(deviceTable114, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel28, "Chop 97-108");
		jPanel28.add(deviceTable115, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel29, "Chop 109-120");
		jPanel29.add(deviceTable116, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel210, "Chop 121-132");
		jPanel210.add(deviceTable117, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel211, "Chop 133-144");
		jPanel211.add(deviceTable118, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel212, "Chop 145-156");
		jPanel212.add(deviceTable119, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel213, "Chop 157-168");
		jPanel213.add(deviceTable1110, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel214, "Chop 169-180");
		jPanel214.add(deviceTable1111, BorderLayout.CENTER);
		jTabbedPane3.add(jPanel215, "Chop 181-192");
		jPanel215.add(deviceTable1112, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel2, "Received Parameters");
		jPanel2.add(jTabbedPane2, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel4, "Chop 1-12");
		jPanel4.add(deviceTable1, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel5, "Chop 13-24");
		jPanel5.add(deviceTable2, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel6, "Chop 25-36");
		jPanel6.add(deviceTable3, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel7, "Chop 37-48");
		jPanel7.add(deviceTable4, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel8, "Chop 49-60");
		jPanel8.add(deviceTable5, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel9, "Chop 61-72");
		jPanel9.add(deviceTable6, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel10, "Chop 73-84");
		jPanel10.add(deviceTable7, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel11, "Chop 85-96");
		jPanel11.add(deviceTable8, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel12, "Chop 97-108");
		jPanel12.add(deviceTable9, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel13, "Chop 109-120");
		jPanel13.add(deviceTable10, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel14, "Chop 121-132");
		jPanel14.add(deviceTable11, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel15, "Chop 133-144");
		jPanel15.add(deviceTable12, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel16, "Chop 145-156");
		jPanel16.add(deviceTable13, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel17, "Chop 157-168");
		jPanel17.add(deviceTable14, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel18, "Chop 169-180");
		jPanel18.add(deviceTable15, BorderLayout.CENTER);
		jTabbedPane2.add(jPanel19, "Chop 181-192");
		jPanel19.add(deviceTable16, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel31, "Choppers");
		jPanel31.add(jScrollPane1, BorderLayout.CENTER);
		jScrollPane1.getViewport().add(deviceTable20, null);
	}
}
